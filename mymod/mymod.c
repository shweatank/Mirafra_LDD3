#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>

#define DEVICE_NAME "mymod_demo"
#define MAJOR 111

struct operation
{
	int a;
	int b;
	int op;
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

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *offset)
{
	if(copy_to_user(user_buf,&s,sizeof(s)))
	{
		return -EFAULT;
	}
	return sizeof(s);
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	if(copy_from_user(&s,user_buf,sizeof(s)))
	{
		return -EFAULT;
	}
	switch(s.op)
	{
		case 1: s.result=s.a+s.b;
			break;

		case 2: s.result=s.a-s.b;
			break;
		case 3: s.result=s.a*s.b;
			break;
		case 4: s.result=s.a/s.b;
			break;
	}
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
	int ret=register_chrdev(MAJOR,DEVICE_NAME,&fops);
	if(ret<0)
	{
		printk("Failed to register the module\n");
		return ret;
	}
	printk("Module registered successfully with major number %d\n",MAJOR);
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
MODULE_DESCRIPTION("A simple module\n");


