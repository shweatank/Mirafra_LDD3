#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/init.h>

#define DEVICE_NAME "enum_dev"
#define MAJOR_NUM 111
enum colour
{
	red=0,
	green=1,
	blue=2
};
static int colour=-1;
static int major;
enum colour c;

static int dev_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "simplle_char_dev:device opened\n");
	return 0;
}
static int dev_release(struct inode *inode,struct file *file)
{
	 printk(KERN_INFO "simplle_char_dev:device closed\n");
        return 0;
}
static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *ppos)
{
	
	if(copy_to_user(user_buf,&c,sizeof(enum colour)))
	{
			return -EFAULT;
	}
	printk(KERN_INFO "enum_char_dev:sent data\n");
	return sizeof(enum colour);
}
static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *ppos)
{
	
	if(count!=sizeof(enum colour))
	{
		printk(KERN_ERR "invalid write size:\n");
		return -EINVAL;
	}
	if(copy_from_user(&c,user_buf,sizeof(enum colour)))
	{
		printk(KERN_ERR "failed to copy enum from user\n");
		return -EFAULT;
	}
	colour=c;

	switch(c)
	{
		case red:
            printk(KERN_INFO "Color set to RED\n");
            break;
		case green:
            printk(KERN_INFO "Color set to GREEN\n");
            break;
        case blue:
            printk(KERN_INFO "Color set to BLUE\n");
            break;
        default:
            printk(KERN_WARNING "Invalid color received: %d\n", c);
        }
	
        return sizeof(enum colour);

}
struct file_operations fops ={
        .open=dev_open,
        .release=dev_release,
        .read=dev_read,
        .write=dev_write,
};
static int __init simple_char_init(void)
{
	major=register_chrdev(MAJOR_NUM,DEVICE_NAME,&fops);
	if(major<0)
	{
	 printk(KERN_ALERT "enum_char_dev:fail to register\n");
        return major;
	}
	printk(KERN_INFO "enum_char_dev:registered with major number %d\n",major);
	return 0;
}
static void __exit simple_char_exit(void)
{
	unregister_chrdev(MAJOR_NUM,DEVICE_NAME);
	 printk(KERN_INFO "ENUM_char_dev:device unregistered andexited\n");
}
MODULE_LICENSE("GPL");
module_init(simple_char_init);
module_exit(simple_char_exit);


