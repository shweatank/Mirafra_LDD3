#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
static int __init divzero_init(void)
{
    int a = 10, b = 0;
    int res;
    printk(KERN_INFO "Division by zero demo: a=%d, b=%d\n", a, b);
    res = a / b;
    printk(KERN_INFO "Result: %d\n", res);
    return 0;
}
static void __exit divzero_exit(void)
{
    printk(KERN_INFO "Division by zero module exiting.\n");
}
module_init(divzero_init);
module_exit(divzero_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("Kernel module to demonstrate division by zero (CRASHES SYSTEM!)");

