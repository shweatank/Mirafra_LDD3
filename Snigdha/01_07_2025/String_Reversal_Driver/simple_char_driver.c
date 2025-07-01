#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "simple_char_dev"
#define BUF_LEN 128

static int major;
static char device_buffer[BUF_LEN];
static int data_len=0;


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

void reversed(char *str,int len)
{
	char temp;
	for(int i=0;i<len/2;i++)
	{
		temp=str[i];
		str[i]=str[len-i-1];
		str[len-i-1]=temp;
	}
}

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *ppos)
{
	if(count > data_len- *ppos)
		count = data_len- *ppos;


	if(copy_to_user(user_buf,device_buffer + *ppos,count ))
	{
		return -EFAULT;
	}
	*ppos += count;


//	printk(KERN_INFO "simple_char_dev: Sent %d to kernel\n",number);

	return count;
}


static ssize_t dev_write(struct file *file, const char __user *user_buf,size_t count, loff_t *ppos)
{
	if(count> BUF_LEN)
		count= BUF_LEN - 1;
	if(copy_from_user(device_buffer,user_buf,count))
		return -EFAULT;

	device_buffer[count]='\0';
	data_len=strlen(device_buffer);
	printk(KERN_INFO "simple_char_dev: Received string %s \n",device_buffer);

	reversed(device_buffer,data_len);

	printk(KERN_INFO "simple_char_dev:Reversed string: %s\n",device_buffer);
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
