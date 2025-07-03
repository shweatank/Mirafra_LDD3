#include<linux/kernel.h>
#include<linux/uaccess.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>

#define MY_MAJOR 121
#define DEVICE_NAME "mymodule"

static int major;
enum day{sunday,monday,tuesday,wednesday,thursday,friday,saturday};
static enum day d;

static int dev_open(struct inode *inode,struct file *file)
{
	printk("mymodule: Device opened\n");
	return 0;
}

static int dev_close(struct inode *inode,struct file *file)
{
	printk("mymodule: Device closed\n");
	return 0;
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	if(copy_from_user(&d,user_buf,sizeof(enum day)))
	{
		return -EFAULT;
	}
	printk("Enum written in kernel space with %ld\n",sizeof(enum day));
	return sizeof(enum day);
}

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *offset)
{
	if(copy_to_user(user_buf,&d,sizeof(enum day)))
	{
		return -EFAULT;
	}
	printk("enum read %ld bytes\n",sizeof(enum day));
	return sizeof(enum day);
}

struct file_operations fops={
	.owner=THIS_MODULE,
	.open=dev_open,
	.release=dev_close,
	.read=dev_read,
	.write=dev_write
};

static int __init mymod_init(void)
{
	major=register_chrdev(MY_MAJOR,DEVICE_NAME,&fops);
	if(major<0)
	{
		printk("Failed to registered module\n");
		return major;
	}
	printk("Module loaded successfully with major number %d\n",MY_MAJOR);
	return 0;
}

static void __exit mymod_exit(void)
{
	unregister_chrdev(MY_MAJOR,DEVICE_NAME);
	printk("Module removed successfully\n");
}

module_init(mymod_init);
module_exit(mymod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("A simple module passing enum");

