#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

static int __init hello_init(void)
{
	printk(KERN_INFO "KERNEL LODADED\n");
	printk(KERN_ERR "KERNEL SHOWING ERROR\n");
	pr_info("info\n");
	pr_debug("debug\n");
	pr_err("error\n");
	pr_warn("warning\n");
	return 0;
}
static void __exit hello_exit(void)
{
	printk(KERN_INFO "good bye to kernel\n");
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sowmya");
MODULE_DESCRIPTION("A SIMPLE HELLO WORLD PROHREM KERNEL MODULE");
