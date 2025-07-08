#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>

static int __init crash_init(void)
{
	printk(KERN_ALERT "crash module\n");
	int a=23,b=0;
	//int result=a/b;
//	printk(KERN_INFO "Result=%d\n",result);
	return 0;
}
static void __exit crash_exit(void)
{
	printk(KERN_ALERT "crash module exist\n");
}

module_init(crash_init);
module_exit(crash_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BHARATH");
MODULE_DESCRIPTION("kernel crash");
