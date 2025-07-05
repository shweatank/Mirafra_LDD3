#include<linux/init.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#define DEVICE_NAME "simple_enum_dev"
//#define CLASS_NAME  "simple_enum"
#define FIXED_MAJOR 150
//static int major;

enum Command {
    CMD_HELLO = 1,
    CMD_BYE   = 2
};

static int result = 0;
static int dev_open(struct inode *inode,struct file*file)
{
	printk(KERN_INFO "simple_char_dev: Device opened\n");
	return 0;
}
static int dev_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "simple_char_dev: device closed\n");
		return 0;
}
static ssize_t dev_read(struct file *file ,char __user *user_buf ,size_t count,loff_t *ppos)
{
	if(copy_to_user(user_buf,&result,sizeof(int)))
	{
			return -EFAULT;
	}
	return sizeof(int);
	
}
static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *ppos)
{
	int user_input;

    if (copy_from_user(&user_input, user_buf, sizeof(int)))
        return -EFAULT;

    switch (user_input) {
        case CMD_HELLO:
            result = 111;
            printk(KERN_INFO "Received CMD_HELLO\n");
            break;
        case CMD_BYE:
            result = 222;
            printk(KERN_INFO "Received CMD_BYE\n");
            break;
        default:
            result = -1;
            printk(KERN_INFO "Unknown command\n");
    }

    return sizeof(int);
}
static struct file_operations fops={
	.open=dev_open,
	.release=dev_release,
	.read=dev_read,
	.write=dev_write,
};
static int __init simple_char_init(void)
{
	//major =register_chrdev(0,DEVICE_NAME,&fops);
	int major=register_chrdev(FIXED_MAJOR,DEVICE_NAME,&fops);
	//int major=0;
	if(major<0)
	{
		printk(KERN_ALERT "Failed to regidter character device\n");
		return  major;
	}
		printk(KERN_INFO "simple_char_dev: registered with major number %d\n",major);
		return 0;
}
static void __exit simple_char_exit(void)
{
	unregister_chrdev(FIXED_MAJOR,DEVICE_NAME);
	printk(KERN_INFO"simple_enum_dev :unregistered device\n");
}
module_init(simple_char_init);
module_exit(simple_char_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adinarayana");
MODULE_DESCRIPTION("Minimual character Device Driver");





