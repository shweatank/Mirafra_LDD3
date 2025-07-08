#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/device.h>

#define DEVICE_NAME "waitq_dev"
#define CLASS_NAME  "waitq"

static int major;
static struct class *cls;

static DECLARE_WAIT_QUEUE_HEAD(waitq);
static char device_buf[100];
static int data_available = 0;

static ssize_t waitq_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    pr_info("waitq_dev: read() - sleeping until data is available\n");

    // Sleep until data_available becomes true
    wait_event_interruptible(waitq, data_available);

    pr_info("waitq_dev: read() - data available, returning to user\n");

    if (copy_to_user(buf, device_buf, len))
        return -EFAULT;

    data_available = 0;  // reset after reading
    return len;
}

static ssize_t waitq_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    if (copy_from_user(device_buf, buf, len))
        return -EFAULT;

    device_buf[len] = '\0'; // ensure null termination
    pr_info("waitq_dev: write() - received: %s\n", device_buf);

    data_available = 1;
    wake_up_interruptible(&waitq);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = waitq_read,
    .write = waitq_write,
};

static int __init waitq_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register character device\n");
        return major;
    }

    cls = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(cls)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(cls);
    }

    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    pr_info("waitq_dev: loaded with major %d\n", major);
    return 0;
}

static void __exit waitq_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("waitq_dev: unloaded\n");
}

module_init(waitq_init);
module_exit(waitq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Character device using wait queue (no cdev)");

