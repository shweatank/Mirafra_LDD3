#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "simple_char_dev"
#define BUF_LEN 128
static int major;
static char device_buffer[BUF_LEN];
static int dev_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "simple_char_dev: Device opened\n");
	return 0;
}
static int dev_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "simple_char_dev: device closed\n");
	return 0;
}
static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *ppos)
{
	//	int bytes_read=strlen(device_buffer);
	        if(count>sizeof(device_buffer)-1)
			count=sizeof(device_buffer)-1;
		if(copy_to_user(user_buf,device_buffer,count)){
			return -EFAULT;
		}
                
//		printk(KERN_INFO "simple_char_dev: %s \n",device_buffer);
		return count;
}
static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *ppos)
{
	if(count > BUF_LEN-1)
		count = BUF_LEN -1;
	if(copy_from_user(device_buffer,user_buf,count)){
		return -EFAULT;
	}
	device_buffer[count]='\0';
        int temp;
	int start=0;
	int end=count-1;
	while(start<end){
	temp=device_buffer[start];
	device_buffer[start]=device_buffer[end];
	device_buffer[end]=temp;
	start++;
	end--;
	}
//	printk(KERN_INFO "Reversed string in kernel:%s\n",device_buffer);
	return count;
}

static struct file_operations fops = {
	.open=dev_open,
	.release=dev_release,
	.read=dev_read,
	.write=dev_write,
};
static int __init simple_char_init(void)
{
	major=register_chrdev(0,DEVICE_NAME,&fops);
	if(major<0){
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
MODULE_AUTHOR("BHARATH");
MODULE_DESCRIPTION("Minimal Character Driver");
	

