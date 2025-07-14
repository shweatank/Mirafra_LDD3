#include <linux/module.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/wait.h>

#define DEVICE_NAME "event_logger"
#define CLASS_NAME "event_class"
#define LOG_BUFFER_SIZE 1024
#define IOCTL_GET_LOG_SIZE _IOR('e', 1, int)

MODULE_LICENSE("GPL");

static int major;
static struct class* event_class = NULL;
static struct device* event_device = NULL;

static char *log_buffer;
static int log_index = 0;
static struct mutex log_mutex;

static DECLARE_WAIT_QUEUE_HEAD(log_wait);
static int data_ready = 0;

// Workqueue
static struct workqueue_struct *event_wq;
static struct work_struct event_work;

// File operations
static long event_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int size;
    switch (cmd) {
        case IOCTL_GET_LOG_SIZE:
            mutex_lock(&log_mutex);
            size = log_index;
            mutex_unlock(&log_mutex);
            if (copy_to_user((int __user *)arg, &size, sizeof(int)))
                return -EFAULT;
            return 0;
        default:
            return -EINVAL;
    }
}

static ssize_t event_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    if (wait_event_interruptible(log_wait, data_ready))
        return -ERESTARTSYS;

    mutex_lock(&log_mutex);
    if (copy_to_user(buf, log_buffer, log_index)) {
        mutex_unlock(&log_mutex);
        return -EFAULT;
    }
    data_ready = 0;
    mutex_unlock(&log_mutex);
    return log_index;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = event_ioctl,
    .read = event_read,
};

// Logging logic
static void log_event_to_buffer(void) {
    mutex_lock(&log_mutex);
    if (log_index + 32 < LOG_BUFFER_SIZE) {
        snprintf(log_buffer + log_index, 32, "Event at jiffies=%lu\n", jiffies);
        log_index += strlen(log_buffer + log_index);
        data_ready = 1;
    }
    mutex_unlock(&log_mutex);
    wake_up_interruptible(&log_wait);
}

static void work_handler(struct work_struct *work) {
    log_event_to_buffer();
}

// Keyboard notifier
static int keyboard_event(struct notifier_block *nblock, unsigned long code, void *_param) {
    struct keyboard_notifier_param *param = _param;
    if (code == KBD_KEYSYM && param->down && param->value == KEY_K) {
        queue_work(event_wq, &event_work);
    }
    return NOTIFY_OK;
}

static struct notifier_block nb = {
    .notifier_call = keyboard_event
};

static int __init event_init(void) {
    int ret;

    log_buffer = kzalloc(LOG_BUFFER_SIZE, GFP_KERNEL);
    if (!log_buffer)
        return -ENOMEM;

    mutex_init(&log_mutex);
    INIT_WORK(&event_work, work_handler);
    event_wq = create_singlethread_workqueue("event_wq");

    ret = register_chrdev(0, DEVICE_NAME, &fops);
    if (ret < 0)
        return ret;
    major = ret;

    event_class = class_create(THIS_MODULE, CLASS_NAME);
    event_device = device_create(event_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    register_keyboard_notifier(&nb);

    printk(KERN_INFO "Event Logger: loaded\n");
    return 0;
}

static void __exit event_exit(void) {
    unregister_keyboard_notifier(&nb);
    flush_workqueue(event_wq);
    destroy_workqueue(event_wq);

    device_destroy(event_class, MKDEV(major, 0));
    class_unregister(event_class);
    class_destroy(event_class);
    unregister_chrdev(major, DEVICE_NAME);

    kfree(log_buffer);
    printk(KERN_INFO "Event Logger: unloaded\n");
}

module_init(event_init);
module_exit(event_exit);

