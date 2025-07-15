#include<linux/kernel.h>
#include<linux/uaccess.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/ioctl.h>
#include<linux/fs.h>
#include<linux/gpio.h>

#define DEVICE_NAME "ioctl_gpio"
#define MAJOR 121
#define MAGIC 124
#define IOCTL_SET _IOW(MAGIC,'1',int)
#define IOCTL_GET _IOR(MAGIC,'2',int)
#define LED 17+512

static int status=0;

static long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
	int temp;
	switch(cmd)
	{
		case IOCTL_SET:
				if(copy_from_user(&temp,(int __user*)arg,sizeof(int)))
					return -EFAULT;
				status=temp;	
				gpio_set_value(LED,status);
				pr_info("Value of status is %d\n",status);
				break;
		case IOCTL_GET:
				if(copy_to_user((int __user*)arg,&status,sizeof(int)))
					return -EFAULT;
				pr_info("status sent %d\n",status);
				break;
		default:
				return -EFAULT;
	}
	return 0;
}
				
struct file_operations fops={
	.unlocked_ioctl=my_ioctl
};
static int __init mymod_init(void)
{
	int ret=register_chrdev(MAJOR,DEVICE_NAME,&fops);
	if(ret<0)
	{
		pr_info("Failed to register module\n");
		return ret;
	}
	pr_info("Module registered successfully with %d major number\n",MAJOR);
	if(!gpio_is_valid(LED))
	{
		pr_err("LED invalid gpio %d\n",LED);
		return -EFAULT;
	}
	gpio_request(LED,"LED");
	gpio_direction_output(LED,0);
	pr_info("GPIO %d initialized for LED control\n",LED);
	return 0;
}

static void __exit mymod_exit(void)
{
	gpio_set_value(LED,0);
	gpio_free(LED);
	unregister_chrdev(MAJOR,DEVICE_NAME);
	pr_info("Module removed successfully\n");
}

module_init(mymod_init);
module_exit(mymod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("author");
MODULE_DESCRIPTION("a simple ioctl gpio module\n");

