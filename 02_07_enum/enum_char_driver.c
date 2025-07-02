#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "mychar"
#define BUF_LEN 128
#define MAJOR 100

//static int result;
static  int num;
//static struct calcu c;
static int result;
static char device_buffer[BUF_LEN];
static int dev_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "simple_char_dev: Device opened\n");
	return 0;
}
static int dev_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "simple_char_dev: Device closed\n");
	return 0;
}
static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *ppos)
{
	int bytes_read= strlen(device_buffer);
	if(copy_to_user(user_buf,&num,4)){
		return -EFAULT;
	}
	printk(KERN_INFO "simple_char_dev: Read %d bytes\n",bytes_read);
	return 4;
}
static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *ppos)
{
	if(count > BUF_LEN-1)
		count=BUF_LEN-1;
	/*if(copy_from_user(device_buffer,user_buf,count)){
		return -EFAULT;
	}*/
	if(copy_from_user(&num,user_buf,sizeof(num))){
		return -EFAULT;
	}
	num++;
	//device_buffer[count]='\0';
	//number++;
	printk(KERN_INFO "simple_char_dev: Written %zu bytes\n",count);
	return 4;
}
static struct file_operations fops = {
	.open=dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

static int __init simple_char_init(void)
{
    result = register_chrdev(MAJOR, DEVICE_NAME, &fops);
    if (result < 0) {
        printk(KERN_ALERT "Failed to register character device\n");
        return result;
    }

    // Check if MAJOR was 0 (dynamic allocation)
    if (MAJOR == 0)
        printk(KERN_INFO "simple_char_dev: Registered with dynamically assigned major number %d\n", result);
    else
        printk(KERN_INFO "simple_char_dev: Registered with fixed major number %d\n", MAJOR);

    return 0;
}

static void __exit simple_char_exit(void)
{
	unregister_chrdev(result,DEVICE_NAME);
	printk(KERN_INFO "simple_char_dev: Unregistered device\n");
}
module_init(simple_char_init);
module_exit(simple_char_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Minimal Character Device Driver");

