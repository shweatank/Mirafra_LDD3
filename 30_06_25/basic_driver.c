#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

MODULE_LICENSE("GPL")
MODULE_AUTHOR("NAME")
MODULE_DESCRIPTION("A Simple Hello world Linux Kernel Module");
static int __init hello_init(void){
	pr_info("Hello, Kernel!\n");
	pr_debug("Hello, Kernel!\n");
	pr_err("Hello, Kernel!\n");
	pr_notice("Hello, Kernel!\n");
	return 0;
}

static void __exit hello_exit(void) {
	printk(KERN_INFO "GOOD BYE,Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
