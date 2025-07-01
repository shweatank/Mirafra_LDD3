#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "simple_char_dev"
#define BUF_LEN 128

static int major;
static int number=0;

static char device_buffer[BUF_LEN];

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
	int bytes_read=strlen(device_buffer);
	char buf[32];
	int len;

	number++;

	len=snprintf(buf,sizeof(buf),"%d",number);


	if(copy_to_user(user_buf,buf,len))
	{
		return -EFAULT;
	}
	*ppos +=len;


	printk(KERN_INFO "simple_char_dev: Sent %d to kernel\n",number);

	return len;
}


static ssize_t dev_write(struct file *file, const char __user *user_buf,size_t count, loff_t *ppos)
{
	char buf[32];
	if(count> sizeof(buf))
		count= sizeof(buf) - 1;
	if(copy_from_user(buf,user_buf,count))
		return -EFAULT;

	buf[count]='\0';
	kstrtoint(buf,10,&number);
	printk(KERN_INFO "simple_char_dev: Written %d \n",number);
	return count;
}

static struct file_operations fops={
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

static int __init simple_char_init(void)
{
	major= register_chrdev(0,DEVICE_NAME,&fops);
	if(major<0)
	{
		printk(KERN_ALERT "Failed to register character device\n");
		return major;
	}

	printk(KERN_INFO "simple_char_dev: Registered with major number %d\n",major);
	return 0;
}


static void __exit simple_char_exit(void)
{
	unregister_chrdev(major,DEVICE_NAME);
	printk(KERN_INFO "simple_char_dev: Unregistered device\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("SNIGDHA");
MODULE_DESCRIPTION("Minimal Character Device Driver");
