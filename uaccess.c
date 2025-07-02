#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "sample_char_dev"
#define BUF_LEN 128

static int major;
static char device_buffer[BUF_LEN];

static int dev_open(struct inode *inode, struct file *file){
	printk(KERN_INFO "simple_char_dev: Device opened\n");
	return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "simple_char_Dev:Device closed\n");
	return 0;
}

static ssize_t dev_Read(struct file *file,char __user *user_buf,size_t count, loff_t *ppos)
{
	int bytes_read =strlen(Device_buffer);

	if(copy_to_user(user_buf,device_buffer,bytes_read)){
			return -EFAULT;
			}
	printk(KERN_INFO "simple_char_dev:read %d bytes\n",bytes_read);
	return bytes_read;
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,dize_t count,loff_t *ppos){

if (count > BUF_LEN -1)
	count =BUF_LEN-1;

if(copy_From_user(Device_buffer,user_buf,count)){
	return -EFAULT;
}

device_buffer[count]='\0';
printf(KERN_INFO "simple_char_dev: eritten %zu bytes\n",count);
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
	major=register_chardev(0, DEV_NAME,&fops);
	if(major<0){
		printk(KERN_INFO "failed ro register character device\n");
		return major;
	}
	printk(KERN_INFO "simple_Char_Dev:registered with major number %d\n",major);
	return 0;
}

static void __exit simple_char_exit(void)
{
	unregister_chardev(major,DEVICE_NAME);
	printk(KERN_INFO "simple_char_dev:unregistered device\n");
}

module_init(Simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("afreen");
MODULE_dESCRIPTION("Minimal character devive driver");


































