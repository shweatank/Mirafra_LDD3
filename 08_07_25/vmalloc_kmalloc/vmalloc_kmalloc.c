#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>         // kmalloc, kfree
#include <linux/vmalloc.h>      // vmalloc, vfree
#include <linux/mm.h>           // page_to_phys
#include <linux/highmem.h>      // kmap
#include <linux/io.h>           // virt_to_phys
#include <linux/types.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kusuma");
MODULE_DESCRIPTION("Compare kmalloc vs vmalloc and print physical addresses");
MODULE_VERSION("1.1");

#define KMALLOC_SIZE (4096 * 4) // 16KB
#define VMALLOC_SIZE (4096 * 4)
#define PAGE_CNT 4

static char *kptr;
static char *vptr;

static int __init phys_virt_addr_init(void)
{
    unsigned long  phys;

    // kmalloc region
    kptr = kmalloc(KMALLOC_SIZE, GFP_KERNEL);
    if (kptr == NULL) {
        pr_err("kmalloc failed\n");
        return -ENOMEM;
    }

    pr_info("kmalloc virtual address: %px\n", kptr);
    for (int i = 0; i < PAGE_CNT; i++) {
        virt = (unsigned long)(kptr + i * PAGE_SIZE);
        phys = virt_to_phys((void *)virt);
        pr_info("kmalloc: page %d: virt = %px, phys = 0x%lx\n", i, (void *)virt, phys);
        /*void *virt =(kptr + i * PAGE_SIZE);
        phys = virt_to_phys(virt);
        pr_info("kmalloc: page %d: virt = %px, phys = 0x%lx\n", i, virt, phys);*/
    }

    // vmalloc region
    vptr = vmalloc(VMALLOC_SIZE);
    if (vptr == NULL) {
        pr_err("vmalloc failed\n");
        kfree(kptr);
        return -ENOMEM;
    }

    pr_info("vmalloc virtual address: %px\n", vptr);
    for (int i = 0; i < PAGE_CNT; i++) {
        void *vaddr = vptr + i * PAGE_SIZE;
        struct page *page = vmalloc_to_page(vaddr);
        phys = page_to_phys(page);
        pr_info("vmalloc: page %d: virt = %px, phys = 0x%lx\n", i, vaddr, phys);
    }

    return 0;
}

static void __exit phys_virt_addr_exit(void)
{
    pr_info("Cleaning up allocated memory\n");
    if (kptr)
        kfree(kptr);
    if (vptr)
        vfree(vptr);
}

module_init(phys_virt_addr_init);
module_exit(phys_virt_addr_exit);

