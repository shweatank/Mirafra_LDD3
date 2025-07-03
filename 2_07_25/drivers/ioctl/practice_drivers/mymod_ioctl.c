#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/ioctl.h>

#define MAJOR_NUM 111
#define DEVICE_NAME "mymod_ioctl"
#define WR_VALUE _IOW(MAJOR_NUM,0,int)
#define RD_VALUE _IOR(MAJOR_NUM,1,int)

static int device_number=0;
static int dev_open(struct inode *inode,struct file *file)
{
	printk("Module opened successfully\n");
	return 0;
}

static int dev_release(struct inode *inode,struct file *file)
{
	printk("Module closed successfully\n");
	return 0;
}

static long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
	int user_val;
	switch(cmd)
	{
		case WR_VALUE:
			if(copy_from_user(&user_val,(int __user *)arg,sizeof(user_val)))
				return -EFAULT;
			device_number=user_val;
			printk("Received number %d\n",device_number);
			break;
		case RD_VALUE:
			if(copy_to_user((int __user *)arg,&device_number,sizeof(device_number)))
				return -EFAULT;
			printk("Sent number %d\n",device_number);
			break;
		default:
			return -EFAULT;
	}
	return 0;
}

struct file_operations fops={
	.open=dev_open,
	.release=dev_release,
	.unlocked_ioctl=my_ioctl
};

static int __init ioctl_init(void)
{
	int ret=register_chrdev(MAJOR_NUM,DEVICE_NAME,&fops);
	if(ret<0)
	{
		printk("Failed to register char device\n");
		return ret;
	}
	printk("Module registered successfully with major number %d\n",MAJOR_NUM);
	return 0;
}

static void __exit ioctl_exit(void)
{
	unregister_chrdev(MAJOR_NUM,DEVICE_NAME);
	printk("Module removed successfully\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("A simple ioctl module");
