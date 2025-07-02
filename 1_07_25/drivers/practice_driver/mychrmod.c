#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>

#define DEVICE_NAME "mychrmod"
#define BUF_LEN 128
static char device_buffer[BUF_LEN];
static int major;


static int dev_open(struct inode *inode, struct file *file)
{
	printk("mychrmod: Device opened\n");
	return 0;
}

static int dev_close(struct inode *inode,struct file *file)
{
	printk("mychrmod: Device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *offset)
{
	int num_of_bytes=strlen(device_buffer);
	if(copy_to_user(user_buf,device_buffer,num_of_bytes))
	{
		return -EFAULT;
	}
	printk("mychrmod: read %d bytes\n",num_of_bytes);
	return num_of_bytes;
}


static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	if(count> BUF_LEN - 1)
	{
		count=BUF_LEN;
	}
	if(copy_from_user(device_buffer,user_buf,count))
	{
		return -EFAULT;
	}
	device_buffer[count]='\0';
	printk("mychrmod: written %zu bytes\n",count);
	return count;
}
static struct file_operations fops={
	.open=dev_open,
	.release=dev_close,
	.read=dev_read,
	.write=dev_write
};

static int __init chrmod_init(void)
{
	major=register_chrdev(0,DEVICE_NAME,&fops);
	if(major<0)
	{
		printk("Failed to load a module\n");
		return major;
	}
	printk("mychrmod: Register with major number %d\n",major);
	return 0;
}

static void __exit chrmod_exit(void)
{
	unregister_chrdev(major,DEVICE_NAME);
	printk("Module unregistered successfully\n");
}

module_init(chrmod_init);
module_exit(chrmod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("A simple module created");
