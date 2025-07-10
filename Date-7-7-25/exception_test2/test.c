#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init exception_init(void)
{
    int a = 10;
    int b = 0;
    int result;

    printk(KERN_INFO "EXCEPTION Module loaded.\n");

    result = a / b;
    printk(KERN_INFO "EXCEPTION Result = %d\n", result);

    return 0;
}

static void __exit exception_exit(void)
{
    printk(KERN_INFO "[EXCEPTION] Module unloaded.\n");
}

module_init(exception_init);
module_exit(exception_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GH");
MODULE_DESCRIPTION("Kernel exception handling - divide by zero");

