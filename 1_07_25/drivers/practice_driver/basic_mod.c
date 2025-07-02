#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
//#include<linux/fs.h>

//#define DEVICE_NAME "mychrmod"

static int __init chrmod_init(void)
{
	printk("Module loaded successfully\n");
	return 0;
}

static void __exit chrmod_exit(void)
{
	printk("Module removed successfully\n");
}

module_init(chrmod_init);
module_exit(chrmod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("A simple module created");
