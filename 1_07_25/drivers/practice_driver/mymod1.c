#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>

#define DEVICE_NAME "mymod1"
#define BUF_LEN 128

static int major;
static char device_buffer[BUF_LEN];

static int dev_open(struct inode *inode,struct file *file)
{
	printk("mymod1: Device opened\n");
	return 0;
}
static int dev_close(struct inode *inode,struct file *file)
{
	printk("mymod1: Device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *offset)
{
	int len=strlen(device_buffer);
	if(copy_to_user(user_buf,device_buffer,len))
	{
		return -EFAULT;
	}
	printk("Data read %d bytes\n",len);
	return len;
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	if(count>BUF_LEN-1)
	{
		count=BUF_LEN;
	}
	if(copy_from_user(device_buffer,user_buf,count))
	{
		return -EFAULT;
	}
	int i,len;
	device_buffer[count]='\0';
	len=strlen(device_buffer);
	for(i=0;i<len/2;i++)
	{
		char temp=device_buffer[i];
		device_buffer[i]=device_buffer[len-1-i];
		device_buffer[len-1-i]=temp;
	}
	printk("Data written %zu bytes\n",count);
	return count;
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
		printk("Failed to register the module\n");
		return major;
	}
	printk("Module registered successfully with major number %d\n",major);
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
