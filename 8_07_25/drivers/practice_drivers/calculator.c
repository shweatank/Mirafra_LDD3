#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/init.h>

#define DEVICE_NAME "calc"
#define MAJOR 111
DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag=0;
struct operation
{
	int x;
	int y;
	int result;
	char op
}s;

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *offset)
{
	printk("Waiting for user to sent input\n");
	wait_event(wq,flag!=0);

	flag=0;
	if(copy_to_user(user_buf,&s,sizeof(s)))
		return -EFAULT;
	return sizeof(s);
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	if(copy_from_user(&s,user_buf,sizeof(s)))
		return -EFAULT;
	switch(s.op)
	{
		case '+': 
			 s.result=s.x+s.y;
			 break;
		case '-': 
			 s.result=s.x-s.y;
			 break;
		case '*': 
			 s.result=s.x*s.y;
			 break;
		case '/': 
			 s.result=s.x/s.y;
			 break;
	}
	flag=1;
	wake_up(&wq);
	return sizeof(s);
}



struct file_operations fops={
	.read=dev_read,
	.write=dev_write
};

static int __init my_cal_init(void)
{
	int ret=register_chrdev(MAJOR,DEVICE_NAME,&fops);
	if(ret<0)
	{
		printk("failed to register the module\n");
		return ret;
	}
	printk("Module loaded successfully with %d major number\n",MAJOR);
	return 0;
}

static void __exit my_cal_exit(void)
{
	printk("Module removed successfully\n");
	unregister_chrdev(MAJOR,DEVICE_NAME);
}

module_init(my_cal_init);
module_exit(my_cal_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple module\n");
