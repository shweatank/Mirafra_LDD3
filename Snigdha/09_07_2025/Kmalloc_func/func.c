// kmalloc_example.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>     // for kmalloc and kfree

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snigdha");
MODULE_DESCRIPTION("kmalloc example");

static char *kmalloc_mem;

static int __init kmalloc_example_init(void)
{
    // Allocate 100 bytes using kmalloc (physically contiguous)
    kmalloc_mem = kmalloc(100, GFP_KERNEL);

    if (!kmalloc_mem) {
        pr_err("kmalloc failed!\n");
        return -ENOMEM;
    }

    strcpy(kmalloc_mem, "Hello from kmalloc!");
    pr_info("kmalloc success: %s\n", kmalloc_mem);
    return 0;
}

static void __exit kmalloc_example_exit(void)
{
    if (kmalloc_mem) {
        pr_info("Freeing kmalloc memory\n");
        kfree(kmalloc_mem);
    }
    pr_info("kmalloc module unloaded\n");
}

module_init(kmalloc_example_init);
module_exit(kmalloc_example_exit);

