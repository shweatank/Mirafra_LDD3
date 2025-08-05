// vmalloc_example.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/vmalloc.h>  // for vmalloc and vfree

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snigdha");
MODULE_DESCRIPTION("vmalloc example");

static char *vmalloc_mem;

static int __init vmalloc_example_init(void)
{
    // Allocate 1 MB using vmalloc (virtually contiguous)
    vmalloc_mem = vmalloc(1024 * 1024);

    if (!vmalloc_mem) {
        pr_err("vmalloc failed!\n");
        return -ENOMEM;
    }

    strcpy(vmalloc_mem, "Hello from vmalloc!");
    pr_info("vmalloc success: %s\n", vmalloc_mem);
    return 0;
}

static void __exit vmalloc_example_exit(void)
{
    if (vmalloc_mem) {
        pr_info("Freeing vmalloc memory\n");
        vfree(vmalloc_mem);
    }
    pr_info("vmalloc module unloaded\n");
}

module_init(vmalloc_example_init);
module_exit(vmalloc_example_exit);

