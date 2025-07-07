#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavan (for testing)");
MODULE_DESCRIPTION("Intentional Kernel Crash Module");

static int __init crash_init(void)
{
    printk(KERN_ALERT "Crashing the system now!\n");

    int a=12,b=0,c;
    c=a/b;

    return 0;
}

static void __exit crash_exit(void)
{
    printk(KERN_INFO "Crash module exit (won't be reached if system panics).\n");
}

module_init(crash_init);
module_exit(crash_exit);

