#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>

#define SIZE 1024

static char *kmem;

static int __init kmalloc_init(void) {
    kmem = kmalloc(SIZE, GFP_KERNEL);
    if (!kmem)
        return -ENOMEM;
    strcpy(kmem, "Hello from kmalloc");
    pr_info("kmalloc data: %s\n", kmem);
    return 0;
}

static void __exit kmalloc_exit(void) {
    if (kmem)
        kfree(kmem);
    pr_info("kmalloc freed\n");
}

module_init(kmalloc_init);
module_exit(kmalloc_exit);

MODULE_LICENSE("GPL");

