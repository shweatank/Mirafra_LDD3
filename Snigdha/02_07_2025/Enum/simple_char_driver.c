#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "simple_char_dev"
#define BUF_LEN 128
#define MAJOR_NUM 222

static int major;
static char device_buffer[BUF_LEN];
static int res=0;
enum 
{
	ADD=1,
	SUB,
	MUL,
	DIV
};


static int dev_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "Simple_char_dev: Device opened\n");
	return 0;
}

static int dev_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "simple_char_dev: Device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *ppos)
{
	if(*ppos > 0)
		return 0;


	if(copy_to_user(user_buf,&res,sizeof(res) ))
	{
		return -EFAULT;
	}
	*ppos += sizeof(res);


//	printk(KERN_INFO "simple_char_dev: Sent %d to kernel\n",number);

	return sizeof(res);
}


static ssize_t dev_write(struct file *file, const char __user *user_buf,size_t count, loff_t *ppos)
{
	int input[3];
	if(count != sizeof(input))
	{
		printk(KERN_ERR "Expected %zu bytes but got %zu bytes\n",sizeof(input),count);
		return -EFAULT;
	}
	if(copy_from_user(input,user_buf,sizeof(input)))
		return -EFAULT;
	

	int a=input[0];
	int b=input[1];
	int op=input[2];

	switch(op)
	{
		case ADD : res=a+b;break;
		case SUB : res=a-b;break;
		case MUL : res=a*b;break;
		case DIV : res=a/b;break;
		default : 
			   printk(KERN_ERR "Unknown operation code: %d\n", op);
            		   return -EINVAL;
	}
	printk(KERN_INFO "Operation %d %d %d = %d\n", a, op, b, res);
        return sizeof(input);
}

static struct file_operations fops={
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

static int __init simple_char_init(void)
{
	major= register_chrdev(MAJOR_NUM,DEVICE_NAME,&fops);
	if(major<0)
	{
		printk(KERN_ALERT "Failed to register character device\n");
		return major;
	}

	printk(KERN_INFO "simple_char_dev: Registered with major number %d\n",MAJOR_NUM);
	return 0;
}


static void __exit simple_char_exit(void)
{
	unregister_chrdev(MAJOR_NUM,DEVICE_NAME);
	printk(KERN_INFO "simple_char_dev: Unregistered device\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("SNIGDHA");
MODULE_DESCRIPTION("Minimal Character Device Driver");
