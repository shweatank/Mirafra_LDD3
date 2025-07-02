#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>

#define DEVICE_NAME "mymod2"
#define LEN 4

static int major;
static int arr[LEN];

static int dev_open(struct inode *inode,struct file *file)
{
	printk("mymod2: Device opened\n");
	return 0;
}

static int dev_close(struct inode *inode,struct file *file)
{
	printk("mymod2: Device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *offset)
{
	int len=4*sizeof(int);
	if(copy_to_user(user_buf,arr,len))
	{
		return -EFAULT;
	}
	printk("Data read %d\n",len);
	return len;
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	int len=3*sizeof(int);
	if(copy_from_user(arr,user_buf,len))
	{
		return -EFAULT;
	}
	int i;
	i=arr[2];
	switch(i)
	{
		case 1: arr[3]=arr[0]+arr[1];
			break;
		case 2: arr[3]=arr[0]-arr[1];
			break;
		case 3: arr[3]=arr[0]*arr[1];
			break;
		case 4: arr[3]=arr[0]/arr[1];
			break;
	}
	printk("Data written\n");
	return len;
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
		printk("Failed to load the module\n");
		return major;
	}
	printk("Module2 registered successfully with major number %d\n",major);
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
MODULE_DESCRIPTION("A simple calculator module");
