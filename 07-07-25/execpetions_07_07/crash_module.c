#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init crash_init(void)
{
    printk(KERN_INFO "Crash module loaded. About to crash the kernel!\n");

   int a=10,b=0;
    if (b == 0) 
    {
        printk(KERN_ERR "Division by zero detected! Skipping operation.\n");
        return -EINVAL;  // Gracefully fail initialization
    }
   int res = a / b;  // This will NOT execute
    printk(KERN_INFO "Result: %d\n", res);

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


