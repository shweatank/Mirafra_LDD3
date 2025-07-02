#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>

#define DEVICE_NAME "mymod"
#define BUF_LEN 128

static int major;
static int number;
static int dev_open(struct inode *inode,struct file *file)
{
	printk("mymod: Device opened\n");
	return 0;
}

static int dev_close(struct inode *inode,struct file *file)
{
	printk("mymod: Device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *offset)
{
	if(copy_to_user(user_buf,&number,sizeof(int)))
	{
		return -EFAULT;
	}
	printk("sent incremented number %d\n",number);
	return sizeof(int);
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	if(copy_from_user(&number,user_buf,sizeof(int)))
	{
		return -EFAULT;
	}
	number++;
	printk("received number and incremented to %d\n",number);
	return sizeof(int);
	
}
struct file_operations fops={
	.open=dev_open,
	.release=dev_close,
	.read=dev_read,
	.write=dev_write
};

static int __init mymod_init(void)
{
	major=register_chrdev(0,DEVICE_NAME,&fops);
	if(major<0)
	{
		printk("Unable to register module\n");
		return major;
	}
	printk("Module registered successfully %d\n",major);
	return 0;
}

static void __exit mymod_exit(void)
{
	unregister_chrdev(major,DEVICE_NAME);
	printk("Module removed successfully\n");
}

module_init(mymod_init);
module_exit(mymod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("A simple module");
