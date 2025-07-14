// Simplified keylogger_driver.c - Only IOCTL + Char Device, no malloc, no IRQ

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "keylogger"

#define KEYLOG_IOCTL_GET_COUNT   _IOR('K', 1, int)
#define KEYLOG_IOCTL_RESET_COUNT _IO('K', 2)
#define KEYLOG_IOCTL_ENABLE      _IO('K', 3)
#define KEYLOG_IOCTL_DISABLE     _IO('K', 4)

static int major;
static int key_count = 0;
static int enabled = 1;

// Only IOCTLs, no read/write
static long keylogger_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case KEYLOG_IOCTL_GET_COUNT:
            if (copy_to_user((int __user *)arg, &key_count, sizeof(int)))
                return -EFAULT;
            break;
        case KEYLOG_IOCTL_RESET_COUNT:
            key_count = 0;
            break;
        case KEYLOG_IOCTL_ENABLE:
            enabled = 1;
            break;
        case KEYLOG_IOCTL_DISABLE:
            enabled = 0;
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static int keylogger_open(struct inode *inode, struct file *file) {
    if (enabled)
        key_count++;
    return 0;
}

static int keylogger_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = keylogger_ioctl,
    .open = keylogger_open,
    .release = keylogger_release,
};

static int __init keylogger_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register char device\n");
        return major;
    }
    printk(KERN_INFO "keylogger: registered with major %d\n", major);
    return 0;
}

static void __exit keylogger_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "keylogger: unregistered\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("IOCTL-only Keypress Counter using Char Dev, no IRQ");

