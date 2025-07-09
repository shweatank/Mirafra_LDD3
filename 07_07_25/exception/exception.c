#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init crash_init(void)
{
    printk(KERN_INFO "Crash module loaded: About to crash!\n");

    // Intentional null pointer dereference (CRASH)
    int *ptr = NULL;
    *ptr = 123;  // This causes a kernel OOPS (crash)

    return 0;
}

static void __exit crash_exit(void)
{
    printk(KERN_INFO "Crash module unloaded.\n");
}

module_init(crash_init);
module_exit(crash_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Crash Test");
MODULE_DESCRIPTION("Kernel module that crashes the system intentionally.");

