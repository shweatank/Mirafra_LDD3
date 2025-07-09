#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init crash_init(void)
{
    printk(KERN_INFO "Crash module loaded. About to crash the kernel!\n");

    //deference of  null pointer 
    int *ptr = NULL;
    *ptr = 42;  // This will trigger a kernel crash (panic)

    return 0;
}

static void __exit crash_exit(void)
{
    printk(KERN_INFO "Crash module unloaded.\n");
}

module_init(crash_init);
module_exit(crash_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kusuma");
MODULE_DESCRIPTION("Module to crash the Linux kernel for testing purposes");


