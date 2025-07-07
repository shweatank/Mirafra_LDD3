#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/ioctl.h>
#include<linux/init.h>
#include<linux/fs.h>

#define DEVICE_NAME "ioctl_demo"
#define MAJOR_NUM 121

#define WR_VALUE _IOW(MAJOR_NUM,0,int)
#define RD_VALUE _IOR(MAJOR_NUM,1,int)

static int device_number=0;

static int dev_open(struct inode *inode,struct file *file)
{
	printk("ioctl demo: device opened\n");
	return 0;
}

static int dev_close(struct inode *inode,struct file *file)
{
	printk("ioctl demo: device closed\n");
	return 0;
}

long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
	int user_val=0;
	switch(cmd)
	{
		case WR_VALUE: 
				if(copy_from_user(&user_val,(int __user *)arg,sizeof(int)))
				       return -EFAULT;
			       	device_number=user_val;
			       	printk("Received number: %d\n",device_number);
			       	break;
		case RD_VALUE:
			       user_val=device_number;	
			       if(copy_to_user((int __user *)arg,&user_val,sizeof(int)))
				       return -EFAULT;
			       printk("Sent number %d\n",device_number);
			       break;
		default: 
			       return -EFAULT;
	}
	return 0;
}

struct file_operations fops=
{
	.open=dev_open,
	.release=dev_close,
	.unlocked_ioctl=my_ioctl
};

static int __init ioctl_init(void)
{
	int ret=register_chrdev(MAJOR_NUM,DEVICE_NAME,&fops);
	if(ret<0)
	{
		printk("Module failed to register\n");
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
MODULE_DESCRIPTION("a simple ioctl module");
