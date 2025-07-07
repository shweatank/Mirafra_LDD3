#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>

#define MAJOR 111
#define DEVICE_NAME "volume_control"

#define IOCTL_SET_VOLUME _IOW(MAJOR,1,int)
#define IOCTL_GET_VOLUME _IOR(MAJOR,2,int)

static int volume=50;//default volume

static long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
	int user_value;
	switch(cmd)
	{
		case IOCTL_SET_VOLUME:
					if(copy_from_user(&user_value,(int32_t *)arg,sizeof(int)))
						return -EFAULT;
					else if(user_value<0)
						volume=0;
					else if(user_value>100)
						volume=100;
					else
						volume=user_value;
					printk("volume set to %d\n",volume);
					break;
		case IOCTL_GET_VOLUME:
					if(copy_to_user((int32_t *)arg,&volume,sizeof(int)))
						return -EFAULT;
					break;
		default:
					return -EFAULT;
	}
	return 0;
}

static int dev_open(struct inode *inode,struct file *file)
{
	printk("volume control: device opened\n");
	return 0;
}

static int dev_release(struct inode *inode,struct file *file)
{
	printk("volume control: device closed\n");
	return 0;
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	char buf[2]={0};
	if(copy_from_user(buf,user_buf,1))
		return -EFAULT;
	if(buf[0]=='+')
	{
		if(volume<100)
			volume++;
		printk("Volume increased to %d\n",volume);
	}
	else if(buf[0]=='-')
	{
		if(volume>0)
			volume--;
		printk("volume decreased to %d\n",volume);
	}
	else
	{
		printk("Unknown write input %c\n",buf[0]);
	}
	return count;
}

struct file_operations fops=
{
	.open=dev_open,
	.release=dev_release,
	.unlocked_ioctl=my_ioctl,
	.write=dev_write
};

static int __init ioctl_init(void)
{
	int ret=register_chrdev(MAJOR,DEVICE_NAME,&fops);
	if(ret<0)
	{
		printk("Failed to register module\n");
		return -EFAULT;
	}
	printk("Module registered successfully with major number %d\n",MAJOR);
	return 0;
}

static void __exit ioctl_exit(void)
{
	unregister_chrdev(MAJOR,DEVICE_NAME);
	printk("Module removed successfully\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple volume controller module");
