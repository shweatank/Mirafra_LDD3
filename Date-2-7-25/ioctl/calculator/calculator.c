#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define MAJOR_NUM 100
#define IOCTL_SET_VALUE _IOW(MAJOR_NUM, 0, int)
#define IOCTL_GET_VALUE _IOR(MAJOR_NUM, 1, int)
#define DEVICE "ioctl_demo"
struct calc_packet
{
    int a;
    int b;
    char op; 
};
struct calc_packet c;
static int result = 0;  

static int add(int a, int b) { return a + b; }
static int sub(int a, int b) { return a - b; }
static int mul(int a, int b) { return a * b; }
static int div(int a, int b) { return a / b; }

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
    int (*fp[])(int,int)={add,sub,mul,div}; 
    int index=-1;    
    switch (cmd) 
    {
        case IOCTL_SET_VALUE:
            if (copy_from_user(&c, (int __user *)arg, sizeof(struct calc_packet))) 
	    {
                return -EFAULT;
            }

            switch(c.op)
            {
                case '+': index = 0; break;
                case '-': index = 1; break;
                case '*': index = 2; break;
                case '/': index = 3; break;
                default:
                    pr_err("Invalid operator: %c\n", c.op);
                    return -EINVAL;
            }

            result = fp[index](c.a, c.b);
            break;	

        case IOCTL_GET_VALUE:
            if (copy_to_user((int __user *)arg, &result, sizeof(int))) 
	    {
                return -EFAULT;
            }
            pr_info("IOCTL: send value = %d\n", result);
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

