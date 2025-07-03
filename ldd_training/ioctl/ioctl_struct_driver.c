#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "ioctl_struct.h"

#define DEVICE_NAME "ioctl_struct"

static int major;
static struct my_data kernel_data;

static int dev_open(struct inode *inode, struct file *file) 
{
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) 
{

    printk(KERN_INFO "Device released\n");
    return 0;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    switch (cmd) 
    {
        case IOCTL_WRITE_DATA:
            if (copy_from_user(&kernel_data, (void __user *)arg, sizeof(kernel_data)))
                return -EFAULT;
            printk(KERN_INFO "User wrote: val1=%d, val2=%d\n", kernel_data.val1, kernel_data.val2);
	    kernel_data.val1+=1;
	    kernel_data.val2+=2;
            break;
        case IOCTL_READ_DATA:
            if (copy_to_user((void __user *)arg, &kernel_data, sizeof(kernel_data)))
                return -EFAULT;
            printk(KERN_INFO "User read: val1=%d, val2=%d\n", kernel_data.val1, kernel_data.val2);
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = 
{
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl,
};

static int __init ioctl_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Registering char device failed\n");
        return major;
    }
    printk(KERN_INFO "Registered with major number %d\n", major);
    printk(KERN_INFO "Use: mknod /dev/%s c %d 0\n", DEVICE_NAME, major);
    return 0;
}

static void __exit ioctl_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Unregistered device\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");

