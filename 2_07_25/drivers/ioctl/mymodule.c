#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>

#define DEVICE_NAME "ioctl_demo"
#define MAJOR_NUM 100

#define IOCTL_SET_NUM _IOW(MAJOR_NUM,0,int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM,1,int)

static int device_number=0;

static int my_open(struct inode *inode,struct file *file)
{
	pr_info("ioctl_demo: device opened\n");
	return 0;
}
static int my_release(struct inode *inode,struct file *file)
{
	pr_info("ioctl_demo: device closed\n");
	return 0;
}

static long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
	int user_val;
	switch(cmd)
	{
		case IOCTL_SET_NUM :
			if(copy_from_user(&user_val,(int __user *)arg,sizeof(user_val)))
				return -EFAULT;
			device_number=user_val;
			pr_info("ioctl demo: Received number %d\n",device_number);
			break;

		case IOCTL_GET_NUM :
			if(copy_to_user((int __user *)arg,&device_number,sizeof(device_number)))
				return -EFAULT;
			pr_info("ioctl demo: sent number %d\n",device_number);
			break;
		default:
			return -EFAULT;
	}
	return 0;
}

static struct file_operations fops={
	.owner=THIS_MODULE,
	.open=my_open,
	.release=my_release,
	.unlocked_ioctl=my_ioctl
};
:x
static int __init ioctl_init(void)
{
	int ret=register_chrdev(MAJOR_NUM,DEVICE_NAME,&fops);
	if(ret<0)
	{
		pr_err("Failed to regiter char device\n");
		return ret;
	}
	pr_info("ioctl_demo: module loaded, major %d\n", MAJOR_NUM);
	return 0;
}

static void __exit ioctl_exit(void)
{
	unregister_chrdev(MAJOR_NUM,DEVICE_NAME);
	pr_info("ioctl demo: module unloaded\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("simple IOCTL driver example");
