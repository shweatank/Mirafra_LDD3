// ioctl() handler function for a character device driver.
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>

#define DEVICE_NAME "ioctl_demo"
#define MAJOR_NUM 101

struct st{
	int a;
	int b;
	char ch;
	int result;
}val;
static struct st device_val;

#define IOCTL_SET_NUM _IOW(MAJOR_NUM,0,int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM,1,int)

//static int device_number=0;

static int my_open(struct inode *inode,struct file *file){
	pr_info("ioctl_demo:device opened\n");
	return 0;
}

static int my_release(struct inode *inode,struct file *file){
	pr_info("ioctl_demo:device closed\n");
	return 0;
}

static long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg){
	//int user_val;
	switch(cmd){
		case IOCTL_SET_NUM:
			if(copy_from_user(&val,(struct st __user*)arg,sizeof(val)))
				return -EFAULT;
			device_val=val;
			switch(device_val.ch){
				case '+':device_val.result=device_val.a+device_val.b;break;
				case '-':device_val.result= device_val.a-device_val.b;break;
				case '*':device_val.result= device_val.a*device_val.b;break;
				case '/':device_val.result=device_val.a/device_val.b;break;
				default:printk("invalid operator\n");
					return 0;
			}
			
			//pr_info("ioctl_Demo:Received number=%d\n",device_number);
			break;
		case IOCTL_GET_NUM:
			if(copy_to_user((struct st __user*)arg,&device_val,sizeof(device_val)))
				return -EFAULT;
			//pr_info("ioctl_demo:sent number=%d\n",device_val);
			break;
		default:
			return -EINVAL;
	}
	return 0;
}
static struct file_operations fops={
	.owner=THIS_MODULE,
	.open=my_open,
	.release=my_release,
	.unlocked_ioctl=my_ioctl,
};

static int __init ioctl_init(void){
	int ret=register_chrdev(MAJOR_NUM,DEVICE_NAME,&fops);
	if(ret<0){
		pr_err("failed to register char device\n");
		return ret;
	}
	pr_info("ioctl_demo:module loade ,major %d\n",MAJOR_NUM);
	return 0;
}

static void __exit ioctl_exit(void){
	unregister_chrdev(MAJOR_NUM,DEVICE_NAME);
	pr_info("ioctl_demo : module unloaded\n");
	
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TECHDABA");
MODULE_DESCRIPTION("simple IOCTL driver");


