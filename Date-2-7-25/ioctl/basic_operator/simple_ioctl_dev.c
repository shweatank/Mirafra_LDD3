#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>


#define MAJOR_NUM 100
#define IOCTL_SET_VALUE _IOW(MAJOR_NUM, 0, int)
#define IOCTL_GET_VALUE _IOR(MAJOR_NUM, 1, int)
#define DEVICE "ioctl_demo"

static int device_value = 0;

static int my_open(struct inode *inode, struct file *file)
{
    pr_info("ioctl_demo: Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info("ioctl_demo: Device closed\n");
    return 0;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int user_val;	
    switch (cmd) 
    {
        case IOCTL_SET_VALUE:
            if (copy_from_user(&user_val, (int __user *)arg, sizeof(user_val))) 
	    {
                return -EFAULT;
            }
	    device_value=user_val;
            pr_info("IOCTL: received value = %d\n", device_value);
            break;

        case IOCTL_GET_VALUE:
            if (copy_to_user((int __user *)arg, &device_value, sizeof(device_value))) 
	    {
                return -EFAULT;
            }
            pr_info("IOCTL: send value = %d\n", device_value);
            break;

        default:
	    {
		pr_info("Invalid Commend:%d\n",cmd);    
            	return -EINVAL;
	    }
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .unlocked_ioctl = my_ioctl,
};

static int __init ioctl_init(void)
{
    int major = register_chrdev(MAJOR_NUM, DEVICE, &fops);
    if (major < 0) {
        pr_err("ioctl: Registration failed\n");
        return major;
    }

    pr_info("ioctl: Registered with major number %d\n", MAJOR_NUM);
    return 0;
}

static void __exit ioctl_exit(void)
{
    unregister_chrdev(MAJOR_NUM, DEVICE);
    pr_info("ioctl: Unregistered\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GH");
MODULE_DESCRIPTION("Character Device Driver with IOCTL Support");

