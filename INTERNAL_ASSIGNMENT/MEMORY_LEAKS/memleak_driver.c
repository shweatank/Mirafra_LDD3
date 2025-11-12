#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

static int __init kmemleak_demo_init(void)
{
    char *mem1, *mem2;

    printk(KERN_INFO "kmemleak_demo: Module loaded\n");

    // Allocate two memory blocks
    mem1 = kmalloc(64, GFP_KERNEL);
    mem2 = kmalloc(128, GFP_KERNEL);

    // Intentionally not freeing mem1 to create a leak
    kfree(mem2);

    printk(KERN_INFO "kmemleak_demo: Allocations done (one leak)\n");
    return 0;
}

static void __exit kmemleak_demo_exit(void)
{
    printk(KERN_INFO "kmemleak_demo: Module unloaded\n");
}

module_init(kmemleak_demo_init);
module_exit(kmemleak_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Simple kernel memory leak demo for kmemleak");

