#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_LICENSE("Sunny Usadadiya");
MODULE_LICENSE("A simple linux driver");

static int __init hello_init(void)
{
    pr_info("Hello, Kernal!\n");
    pr_debug("Hello, Kernal!\n");
    pr_err("Hello, Kernal!\n");
    pr_warn("Hello, Kernal!\n");
    pr_notice("Hello, Kernal!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye, Kernal!\n");
}

module_init(hello_init);
module_exit(hello_exit);
