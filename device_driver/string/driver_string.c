#include<linux/init.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#define DEVICE_NAME "simple_string_dev"
#define BUF_LEN 128
static int major;
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
	int len=strlen(device_buffer);
	if(*ppos>=len)
		return 0;
	if(copy_to_user(user_buf,device_buffer,len))
	{
		return -EFAULT;
	}
	*ppos+=len;
	 printk(KERN_INFO "simple_char_dev: read %d bytes\n", len);
        return len;
}
static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *ppos)
{
	int i,len;
	if(count >BUF_LEN-1)
		count=BUF_LEN-1;
	if(copy_from_user(device_buffer,user_buf,count))
	{
		return -EFAULT;
	}
	device_buffer[count]='\0';
	len = strlen(device_buffer);
    for (i = 0; i < len / 2; i++) {
        char temp = device_buffer[i];
        device_buffer[i] = device_buffer[len - 1 - i];
        device_buffer[len - 1 - i] = temp;
    }
	printk(KERN_INFO "simple_char_dev: written %zu bytes\n",count);
	return count;
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





