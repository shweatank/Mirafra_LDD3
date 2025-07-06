#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>

#define DEVICE_NAME "ioctl_calculator"
#define MAJOR_NUM 100

#define IOCTL_SET_NUM _IOW(MAJOR_NUM,0,int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM,1,int)
/*int add(int a,int b)
{
        return a+b;
}
int sub(int a,int b)
{
        return a-b;
}
int mul(int a,int b)
{
        return a*b;
}
int div(int a,int b)
{
        return a/b;
}*/

struct data 
{
	int a;
	int b;
	char op[4];
};
static struct data calcu;
static int result;

static int calculator(struct data c)
{
	if(strcmp(c.op,"add")==0)
	{
		return c.a+c.b;
	}
	else if(strcmp(c.op,"sub")==0)
        {
                return c.a-c.b;
        }
	else if(strcmp(c.op,"mul")==0)
        {
                return c.a*c.b;
        }
	else if(strcmp(c.op,"div")==0)
        {
                return c.a/c.b;
        }
	else
		return 0;
}

//static int device_number=0;
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
	//int user_val;
	switch(cmd)
	{
		case IOCTL_SET_NUM:
			if(copy_from_user(&calcu,(int __user *)arg,sizeof(calcu)))
				return -EFAULT;
			result=calculator(calcu);
			pr_info("ioctl_demo: Received number = %d  %d\n",calcu.a,calcu.b);
			break;
		case IOCTL_GET_NUM:
			if(copy_to_user((int __user *)arg,&result,sizeof(result)))
				return -EFAULT;
			pr_info("ioctl_demo: Sent number = %d\n",result);
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

static struct file_operations fops ={
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.unlocked_ioctl = my_ioctl,
};

static int __init ioctl_init(void)
{
	int ret=register_chrdev(MAJOR_NUM,DEVICE_NAME,&fops);
	if(ret<0)
	{
		printk(KERN_ALERT"Failed to regoster char device\n");
		return ret;
	}
	printk(KERN_INFO"The device ioctl_demo registered with major number = %d\n",MAJOR_NUM);
	return 0;
}

static void __exit ioctl_exit(void)
{
	unregister_chrdev(MAJOR_NUM,DEVICE_NAME);
	printk(KERN_INFO"ioctl_demo:device unregistered\n");
}
module_init(ioctl_init);
module_exit(ioctl_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("pavan");
MODULE_DESCRIPTION("Simple IOCTL Driver Example");
