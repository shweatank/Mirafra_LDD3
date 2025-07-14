#include <linux/module.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/kernel.h>

#define SIZE 1024

static char *vmem;

static int __init vmalloc_init(void) {
    vmem = vmalloc(SIZE);
    if (!vmem)
        return -ENOMEM;
    strcpy(vmem, "Hello from vmalloc");
    pr_info("vmalloc data: %s\n", vmem);
    return 0;
}

static void __exit vmalloc_exit(void) {
    if (vmem)
        vfree(vmem);
    pr_info("vmalloc freed\n");
}

module_init(vmalloc_init);
module_exit(vmalloc_exit);

MODULE_LICENSE("GPL");

