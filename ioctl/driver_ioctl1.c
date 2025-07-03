#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "ioctll"
#define MAJOR_NUM   100

#define IOCTL_SET_NUM _IOW(MAJOR_NUM, 0, int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM, 1, int)

static int device_number = 0;

static int my_open(struct inode *inode, struct file *file)
{
    pr_info("ioctl: device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info("ioctl: device closed\n");
    return 0;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int user_val;

    switch (cmd) {
    case IOCTL_SET_NUM:
        if (copy_from_user(&user_val, (int __user *)arg, sizeof(user_val)))
            return -EFAULT;
        device_number = user_val;
        pr_info("ioctl: received number = %d\n", device_number);
        break;

    case IOCTL_GET_NUM:
        user_val = device_number;
        if (copy_to_user((int __user *)arg, &user_val, sizeof(user_val)))
            return -EFAULT;
        pr_info("ioctl: sent number = %d\n", device_number);
        break;

    default:
        return -EFAULT; 
    }

    return 0;
}

static const struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = my_open,
    .release        = my_release,
    .unlocked_ioctl = my_ioctl,
};

static int __init ioctl_init(void) {
    int ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (ret < 0) {
        pr_err("ioctl: failed to register device (major %d)\n", MAJOR_NUM);
        return ret;
    }
    pr_info("ioctl: module loaded, major = %d\n", MAJOR_NUM);
    return 0;
}

static void __exit ioctl_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    pr_info("ioctl: module unloaded\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("Simple ioctl driver for setting/getting an integer");

