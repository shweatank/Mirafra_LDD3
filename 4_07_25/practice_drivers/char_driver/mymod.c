#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/init.h>

#define DEVICE_NAME "mymod"
#define MAJOR 111

struct operation
{
	int a;
	int b;
	char op;
	int result;
}s;

static int dev_open(struct inode *inode,struct file *file)
{
	printk("mymod: Device opened\n");
	return 0;
}

static int dev_close(struct inode *inode,struct file *file)
{
	printk("mymod: Device closed\n");
	return 0;
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	if(copy_from_user(&s,user_buf,sizeof(s)))
	{
		return -EFAULT;
	}
	switch(s.op)
	{
		case '+':s.result=s.a+s.b;
			 break;
		case '-':s.result=s.a-s.b;
			 break;
		case '*':s.result=s.a*s.b;
			 break;
		case '/':s.result=s.a/s.b;
			 break;
		default: return -EFAULT;
	}
	printk("Data written to kernel space\n");
	return sizeof(s);
}

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *offset)
{
	if(copy_to_user(user_buf,&s,sizeof(s)))
	{
		return -EFAULT;
	}
	printk("Data read from kernel space\n");
	return sizeof(s);
}

struct file_operations fops=
{
	.open=dev_open,
	.release=dev_close,
	.read=dev_read,
	.write=dev_write
};

static int __init mymod_init(void)
{
	int result=register_chrdev(MAJOR,DEVICE_NAME,&fops);
	if(result<0)
	{
		printk("Failed to register module\n");
		return result;
	}
	printk("Module registered successfully %d\n",MAJOR);
	return 0;
}

static void __exit mymod_exit(void)
{
	unregister_chrdev(MAJOR,DEVICE_NAME);
	printk("Module removed successfully\n");
}

module_init(mymod_init);
module_exit(mymod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("A simple character module\n");
