#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/wait.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define LEN 32
#define DEVICE_NAME "waitqueue_demo"
static int condition=0;
static int ret;
DECLARE_WAIT_QUEUE_HEAD(my_wq);
static char buf[LEN]="Hello from kernel";

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *offset)
{
	wait_event(my_wq,condition!=0);
	if(copy_to_user(user_buf,buf,strlen(buf)))
		return -EFAULT;
	pr_info("Data returning to user\n");
	condition=0;
	return strlen(buf);
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	pr_info("write request sent\n");
	condition=1;
	wake_up(&my_wq);
	return LEN;
}

struct file_operations fops=
{
	.read=dev_read,
	.write=dev_write
};

static int __init mymod_init(void)
{
	ret=register_chrdev(0,DEVICE_NAME,&fops);
	if(ret<0)
	{
		pr_err("Failed to register module\n");
		return ret;
	}
	pr_info("Module registered successfully with %d major number\n",ret);
	return 0;
}

static void __exit mymod_exit(void)
{
	unregister_chrdev(ret,DEVICE_NAME);
	pr_info("Module removed successfully\n");
}

module_init(mymod_init);
module_exit(mymod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple wait queue module\n");

