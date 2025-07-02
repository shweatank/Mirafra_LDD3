#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "simple_char_dev"
#define BUF_LEN 128

static int major;
static char device_buffer[BUF_LEN];

static int dev_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "Simple_char_dev: Device opened\n");
	return 0;
}

static int dev_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "Simple_char_dev: Device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *file,char __user *user_buf,size_t count,loff_t *ppos)
{
	int bytes_read=4*sizeof(int);
	if(copy_to_user(user_buf,device_buffer,bytes_read))
	{
		return -EFAULT;
	}
	printk(KERN_INFO "simple_char_dev: Read %d bytes\n",bytes_read);
	return bytes_read;
	/*int val=0;
	memcpy(&val,device_buffer,sizeof(int));
	val++;
	if(copy_to_user(user_buf,&val,sizeof(int)))
	{
		return -EFAULT;
	}
	return (int *)val;*/
}

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *ppos)
{
	/*if(count > BUF_LEN-1)
		count=BUF_LEN-1;
	if(copy_from_user(device_buffer,user_buf,count))
	{
		return -EFAULT;
	}
	device_buffer[count]='\0';
	printk(KERN_INFO "simple_char_dev: written %zu bytes\n",count);
	return count;*/
	/*if(copy_from_user(device_buffer,user_buf,count))
	{
		return -EFAULT;
	}
	device_buffer[count]='\0';
	int len,i;
	len=strlen(device_buffer);
	for(i=0;i<len/2;i++)
	{
		char temp=device_buffer[i];
		device_buffer[i]=device_buffer[len-1-i];
		device_buffer[len-1-i]=temp;
	}
	printk(KERN_INFO "simple_char_dev: written %zu bytes\n",count);
	return count;*/
	if(count > BUF_LEN-1)
		count=BUF_LEN-1;
	if(copy_from_user(device_buffer,user_buf,count))
	{
		return -EFAULT;
	}
	int i,*data;
	data=(int *)device_buffer;
	i=data[2];
	switch(data[2])
	{
		case 1: data[3]=data[0]+data[1];
			break;
		case 2: data[3]=data[0]-data[1];
			break;
		case 3: data[3]=data[0]*data[1];
			break;
		case 4: data[3]=data[0]/data[1];
			break;
	}
	printk("Data written\n");
	return count;
}

static struct file_operations fops={
	.open=dev_open,
	.release=dev_release,
	.read=dev_read,
	.write=dev_write
};

static int __init simple_char_init(void)
{
	major=register_chrdev(0,DEVICE_NAME,&fops);
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
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("Minimal Character Device Driver");
