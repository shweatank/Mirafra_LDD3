#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/gpio.h>
#include<linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple gpio module");

#define LED 16+512
#define DEVICE_NAME "gpio_demo"
#define CLASS_NAME "gpio_class"
#define MAJOR 123

static struct class *cls;
static struct device *device;
static int value;

static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t count,loff_t *offset)
{
	int temp;
	if(copy_from_user(&temp,user_buf,sizeof(int)))
		return -EFAULT;
	value=temp;
	gpio_set_value(LED,value);
	return sizeof(int);
}

static struct file_operations fops={
	.write=dev_write
};

static int __init mymod_init(void)
{
	int ret=register_chrdev(MAJOR,DEVICE_NAME,&fops);
	if(ret<0)
	{
		pr_err("Failed to register module\n");
		return ret;
	}
	pr_info("Module registered successfully\n");
	cls=class_create(THIS_MODULE,CLASS_NAME);
	if(IS_ERR(cls))
	{
		unregister_chrdev(MAJOR,DEVICE_NAME);
		pr_err("Failed to register the class\n");
		return PTR_ERR(cls);
	}
	device=device_create(cls,NULL,MKDEV(MAJOR,0),NULL,DEVICE_NAME);
	if(IS_ERR(device))
	{
		class_destroy(cls);
		unregister_chrdev(MAJOR,DEVICE_NAME);
		pr_err("Failed to create device\n");
		return PTR_ERR(device);
	}
	if(!gpio_is_valid(LED))
	{
		pr_err("GPIO is invalid\n");
		return -EFAULT;
	}
	gpio_request(LED,"LED");
	gpio_direction_output(LED,1);
	return 0;
}

static void __exit mymod_exit(void)
{
	unregister_chrdev(MAJOR,DEVICE_NAME);
	device_destroy(cls,MKDEV(MAJOR,0));
	class_destroy(cls);
	gpio_free(LED);
	pr_info("Module removed successfully\n");
}

module_init(mymod_init);
module_exit(mymod_exit);
