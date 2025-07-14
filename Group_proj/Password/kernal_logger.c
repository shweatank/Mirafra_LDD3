// =================== kernel_logger.c ===================
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/timekeeping.h>
#include <linux/string.h>

#define DEVICE_NAME "event_logger"
#define CLASS_NAME "event_class"
#define LOG_MAX 100
#define IOCTL_GET_LOGS _IOR('e', 1, char *)
#define IOCTL_DO_LOGIN _IOW('e', 2, char *)

static int major;
static struct cdev logger_cdev;
static struct class *event_class;
static struct device *event_device;

static struct semaphore login_sem;
static char *log_buffer[LOG_MAX];
static int log_count = 0;

static DECLARE_WAIT_QUEUE_HEAD(event_waitq);
static struct workqueue_struct *wq;
static DECLARE_WORK(log_work, NULL);

#define USERNAME "admin"
#define PASSWORD "admin123"

static void log_worker_fn(struct work_struct *work) {
    struct timespec64 ts;
    char *entry;

    ktime_get_real_ts64(&ts);
    entry = kmalloc(128, GFP_KERNEL);
    if (!entry) return;

    snprintf(entry, 128, "Event @ %lld.%09ld\n", (s64)ts.tv_sec, ts.tv_nsec);

    if (log_count < LOG_MAX)
        log_buffer[log_count++] = entry;

    wake_up_interruptible(&event_waitq);
}

static void kb_event(struct input_handle *handle, unsigned int type, unsigned int code, int value) {
    if (type == EV_KEY && value == 1)
        queue_work(wq, &log_work);
}

static int kb_connect(struct input_handler *handler, struct input_dev *dev,
                      const struct input_device_id *id) {
    struct input_handle *handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle) return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "kb_handle";

    if (input_register_handle(handle) || input_open_device(handle)) {
        kfree(handle);
        return -ENODEV;
    }

    return 0;
}

static void kb_disconnect(struct input_handle *handle) {
    input_unregister_handle(handle);
    input_close_device(handle);
    kfree(handle);
}

static const struct input_device_id kb_ids[] = {
    { .driver_info = 1 }, { }
};
MODULE_DEVICE_TABLE(input, kb_ids);

static struct input_handler kb_handler = {
    .event = kb_event,
    .connect = kb_connect,
    .disconnect = kb_disconnect,
    .name = "kb_logger",
    .id_table = kb_ids,
};

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    if (cmd == IOCTL_GET_LOGS) {
        char buffer[4096] = {0};
        int i;
        for (i = 0; i < log_count; i++)
            strcat(buffer, log_buffer[i]);

        if (copy_to_user((char __user *)arg, buffer, strlen(buffer) + 1))
            return -EFAULT;
    } else if (cmd == IOCTL_DO_LOGIN) {
        char user_input[128];
        char uname[64], pwd[64];

        if (copy_from_user(user_input, (char __user *)arg, sizeof(user_input)))
            return -EFAULT;

        sscanf(user_input, "%s %s", uname, pwd);

        down(&login_sem);
        int valid = strcmp(uname, USERNAME) == 0 && strcmp(pwd, PASSWORD) == 0;
        up(&login_sem);

        return valid ? 0 : -1;
    }
    return -EINVAL;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = dev_ioctl,
};

static int __init logger_init(void) {
    dev_t dev;
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    major = MAJOR(dev);
    cdev_init(&logger_cdev, &fops);
    cdev_add(&logger_cdev, dev, 1);

    event_class = class_create(CLASS_NAME);
    if (IS_ERR(event_class)) return PTR_ERR(event_class);

    event_device = device_create(event_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(event_device)) return PTR_ERR(event_device);

    sema_init(&login_sem, 1);
    INIT_WORK(&log_work, log_worker_fn);
    wq = create_workqueue("log_wq");
    input_register_handler(&kb_handler);

    printk(KERN_INFO "[Logger] Module loaded\n");
    return 0;
}

static void __exit logger_exit(void) {
    int i;
    flush_workqueue(wq);
    destroy_workqueue(wq);

    device_destroy(event_class, MKDEV(major, 0));
    class_destroy(event_class);
    unregister_chrdev_region(MKDEV(major, 0), 1);
    cdev_del(&logger_cdev);

    for (i = 0; i < log_count; i++)
        kfree(log_buffer[i]);

    printk(KERN_INFO "[Logger] Module unloaded\n");
}

module_init(logger_init);
module_exit(logger_exit);
MODULE_LICENSE("GPL");
