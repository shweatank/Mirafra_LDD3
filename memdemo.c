#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>

static int __init memdemo_init(void) {
    char *a = kmalloc(1024, GFP_KERNEL);
    char *b = kzalloc(1024, GFP_KERNEL);
    char *c = vmalloc(1024 * 1024);
    unsigned long d = __get_free_pages(GFP_KERNEL, 2);

    printk(KERN_INFO "kmalloc addr: %p\n", a);
    printk(KERN_INFO "kzalloc addr: %p\n", b);
    printk(KERN_INFO "vmalloc addr: %p\n", c);
    printk(KERN_INFO "__get_free_pages addr: %lx\n", d);

    if (a) kfree(a);
    if (b) kfree(b);
    if (c) vfree(c);
    if (d) free_pages(d, 2);

    return 0;
}

static void __exit memdemo_exit(void) {
    printk(KERN_INFO "Exiting memdemo\n");
}

module_init(memdemo_init);
module_exit(memdemo_exit);
MODULE_LICENSE("GPL");
