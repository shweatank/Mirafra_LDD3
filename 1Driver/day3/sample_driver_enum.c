#include<linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define MAJOR_NUM 320
#define DEVICE_NAME "enum_device"
#define IOCTL_MAGIC 0xF0
#define IOCTL_SEND_MODE _IOW(IOCTL_MAGIC,1,device_mode_t)
#define IOCTL_GET_MODE _IOR(IOCTL_MAGIC,2,device_mode_t)

typedef enum
{
        MODE_OFF = 0,Mode_ON,MODE_ERROR
}device_mode_t;

static int major;
static device_mode_t current_mode = MODE_OFF;

static long enum_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_SEND_MODE:
            if (copy_from_user(&current_mode, (device_mode_t __user *)arg, sizeof(current_mode)))
                return -EFAULT;
            printk(KERN_INFO "Received mode from user: %d\n", current_mode);
            break;

        case IOCTL_GET_MODE:
            if (copy_to_user((device_mode_t __user *)arg, &current_mode, sizeof(current_mode)))
                return -EFAULT;
            printk(KERN_INFO "Returned mode to user: %d\n", current_mode);
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .unlocked_ioctl = enum_ioctl,
    .owner = THIS_MODULE,
};

static int __init enum_init(void) {
    major = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register device\n");
        return major;
    }
    printk(KERN_INFO "Enum driver loaded with major %d\n", major);
    return 0;
}

static void __exit enum_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Enum driver unloaded\n");
}

module_init(enum_init);
module_exit(enum_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Enum IOCTL Example");

