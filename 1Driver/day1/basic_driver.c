#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("A simple Hello world Kernel Module");

static int __init hello_init(void)
{
	pr_info("Hello, Kernel!\n");
	pr_debug("Hello, Kernel!\n");
	pr_err("Hello,Kerenl!\n");
	pr_warn("Hello,Kerenl!\n");
	pr_notice("Hello,Kerenl!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Good Bye, Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
