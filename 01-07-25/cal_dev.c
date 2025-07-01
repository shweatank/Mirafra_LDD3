#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "cal_dev"
#define BUF_LEN 128
static int major;
static char device_buffer[BUF_LEN];
struct data_t{
	int a;
	int b;
	char op;
	int result;
};
	struct data_t data;
static int dev_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "cal_dev: Device opened\n");
	return 0;
}
static int dev_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "cal_dev: device closed\n");
	return 0;
}
static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *ppos)
{
	        if(count>sizeof(device_buffer)-1)
			count=sizeof(device_buffer)-1;
		if(copy_to_user(user_buf,&data,sizeof(struct data_t))){
			return -EFAULT;
		}
                
		printk(KERN_INFO "cal_dev: %s \n",device_buffer);
		return count;
}
static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *ppos)
{
	if(count < sizeof(struct data_t)){
			pr_err("Invalid data size\n");
			return -EFAULT;
			}
		
	if(copy_from_user(&data,user_buf,sizeof(struct data_t))){
		return -EFAULT;
	}
	
	printk(KERN_INFO "Data received from user:%d %d %c\n",data.a,data.b,data.op);
	switch(data.op){
		case 'a':data.result=data.a+data.b; break;
	        case 'b':data.result=data.a-data.b; break;
                case 'c':data.result=data.a*data.b; break;
                case 'd':data.result=data.a/data.b; break;
	}
	
	return sizeof(struct data_t);
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
	printk(KERN_INFO "cal_dev: Registered with major number %d\n",major);
	return 0;
}
static void __exit simple_char_exit(void)
{
	unregister_chrdev(major,DEVICE_NAME);
	printk(KERN_INFO "cal_dev: Unregistered device\n");
}
module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BHARATH");
MODULE_DESCRIPTION("Minimal Character Driver");
	

