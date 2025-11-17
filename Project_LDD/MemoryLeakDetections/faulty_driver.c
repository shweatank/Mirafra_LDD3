#include <linux/module.h>
#include <linux/slab.h>

static void *leaked_ptr;

static int __init kmemleak_demo_init(void)
{
    pr_info("kmemleak_demo: init\n");

    /* allocate 128 bytes and then drop our only reference -> leak */
    leaked_ptr = kmalloc(128, GFP_KERNEL);
    pr_info("kmemleak_demo: allocated %p\n", leaked_ptr);

    /* intentionally drop reference to simulate a leak */
    leaked_ptr = NULL;

    return 0;
}

static void __exit kmemleak_demo_exit(void)
{
    pr_info("kmemleak_demo: exit\n");
    /* no kfree() -> allocation is leaked from kernel’s point of view */
}

module_init(kmemleak_demo_init);
module_exit(kmemleak_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Example");
