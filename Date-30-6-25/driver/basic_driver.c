#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("My_Driver");
MODULE_DESCRIPTION("THE BASIC DRIVER");

static int __init hello_init(void)
{
	pr_info("Hello, kernel!\n");
	pr_debug("Hello, kernel!\n");
	pr_err("Hello, kernel!\n");
	pr_warn("Hello, kernel!\n");
	pr_notice("Hello, kernel!\n");
	return 0;
}
static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye, kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);

