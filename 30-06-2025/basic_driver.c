#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priyalatha");
MODULE_DESCRIPTION("A simple Hello World LINUX kernel Module");

static int __init hello_init(void)
{
	pr_info("Hello, Kernel!\n");
	pr_debug("Hello, Kernel!\n");
	pr_err("Hello, Kernel!\n");
	pr_warn("Hello, Kernel!\n");
	pr_notice("Hello, Kernel!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "GoodBye, Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);

