#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/ioctl.h>
#include<linux/fs.h>
#include<linux/init.h>

#define MAJOR_NUM 150
#define DEVICE_NAME "ioctl_calculator"

int val1,val2,result;
#define WR_VALUE1 _IOW(MAJOR_NUM,0,int)
#define WR_VALUE2 _IOW(MAJOR_NUM,1,int)
#define RD_VALUE _IOR(MAJOR_NUM,2,int)

static int dev_open(struct inode *inode,struct file *file)
{
	printk("ioctl calculator: Device opened\n");
	return 0;
}

static int dev_close(struct inode *inode,struct file *file)
{
	printk("ioctl calculator: Device closed\n");
	return 0;
}

static long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		case WR_VALUE1: 
				if(copy_from_user(&val1,(int32_t *)arg,sizeof(int)))
					return -EFAULT;
				printk("value of WR_VALUE1 %d\n",WR_VALUE1);
				printk("Data written to value1\n");
				break;
		case WR_VALUE2:
				if(copy_from_user(&val2,(int32_t *)arg,sizeof(int)))
					return -EFAULT;
				printk("value of WR_VALUE2 %d\n",WR_VALUE2);
				printk("Data written to value2\n");
				break;
		case RD_VALUE:
				result=val1+val2;
				if(copy_to_user((int32_t *)arg,&result,sizeof(int)))
					return -EFAULT;
				printk("value of RD_VALUE %d\n",RD_VALUE);
				printk("Result done\n");
				break;
		default:
				return -EFAULT;
	}
	return 0;
}

struct file_operations fops={
	.open=dev_open,
	.release=dev_close,
	.unlocked_ioctl=my_ioctl
};

static int __init mymod_init(void)
{
	int result=register_chrdev(MAJOR_NUM,DEVICE_NAME,&fops);
	if(result<0)
	{
		printk("Failed to register the module\n");
		return result;
	}
	printk("Module registered successfully with major number %d\n",MAJOR_NUM);
	return 0;
}

static void __exit mymod_exit(void)
{
	unregister_chrdev(MAJOR_NUM,DEVICE_NAME);
	printk("Module removed successfully\n");
}

module_init(mymod_init);
module_exit(mymod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("A simple ioctl module which performs calculation");
