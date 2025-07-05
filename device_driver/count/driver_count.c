
#include<linux/init.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#define DEVICE_NAME "simple_count_dev"
#define BUF_LEN 128
static int major;
static int value=0;
static char device_buffer[BUF_LEN];
static int dev_open(struct inode *inode,struct file*file)
{
	printk(KERN_INFO "simple_char_dev: Device opened\n");
	return 0;
}
static int dev_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "simple_char_dev: device closed\n");
		return 0;
}
static ssize_t dev_read(struct file *file ,char __user *user_buf ,size_t count,loff_t *ppos)
{
	if(*ppos>=count)
		return 0;
	int bytes_read=strlen(device_buffer);

	if(copy_to_user(user_buf,&value,sizeof(value)))
	{
		return -EFAULT;
	}
	*ppos+=count;;
	printk("KERN_INFO :simple_char_dev: read %d bytes\n",bytes_read);
	return  sizeof(value);
}
static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *ppos)
{
	if(count >BUF_LEN-1)
		count=BUF_LEN-1;

	if(copy_from_user(&value,user_buf,sizeof(value)))
	{
		return -EFAULT;
	}
	device_buffer[count]='\0';
	printk(KERN_INFO "simple_char_dev: written %zu bytes\n",count);
	value=value+1;
	return sizeof(value);
}
static struct file_operations fops={
	.open=dev_open,
	.release=dev_release,
	.read=dev_read,
	.write=dev_write,
};
static int __init simple_char_init(void)
{
	major =register_chrdev(0,DEVICE_NAME,&fops);
	if(major<0)
	{
		printk(KERN_ALERT "Failed to regidter character device\n");
		return  major;
	}
		printk(KERN_INFO "simple_char_dev: registered with major number %d\n",major);
		return 0;
}
static void __exit simple_char_exit(void)
{
	unregister_chrdev(major,DEVICE_NAME);
	printk(KERN_INFO"simple_char_dev :unregistered device\n");
}
module_init(simple_char_init);
module_exit(simple_char_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adinarayana");
MODULE_DESCRIPTION("Minimual character Device Driver");





