#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adinarayana");
MODULE_DESCRIPTION("A simple Hello Linux Kernel MOdule");


static int __init hello_init(void)
{
	pr_info("hello,kernel!\n");
	pr_debug("Hello, kernel!\n");
	pr_err("Hello ,kernel!\n");
	pr_warn("Hello, kernel!\n");
	pr_notice("Hello,Kernel!\n");
	return 0;
}
static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye,Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);


