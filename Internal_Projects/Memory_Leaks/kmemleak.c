// kmemleak_test.c

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h> // Required for kmalloc/kfree

// Define the size of our leak (e.g., 256 bytes)
#define LEAK_SIZE 256

// Global pointer to hold the allocated memory
static void *leaked_ptr;

static int __init kmemleak_test_init(void)
{
    // 1. Allocation in kernel space using kmalloc
    leaked_ptr = kmalloc(LEAK_SIZE, GFP_KERNEL);
    
    if (!leaked_ptr) {
        pr_err("kmemleak_test: Failed to allocate memory.\n");
        return -ENOMEM;
    }
    
    // 2. Memory is initialized
    memset(leaked_ptr, 0xAA, LEAK_SIZE);

    pr_info("kmemleak_test: Module loaded. Leaking %d bytes at address %px\n", 
            LEAK_SIZE, leaked_ptr);

    // Note: We do NOT call kmemleak_not_leak(leaked_ptr) here
    return 0;
}

static void __exit kmemleak_test_exit(void)
{
    // 3. The leak: We intentionally omit the kfree() call
    // kfree(leaked_ptr); // <--- We intentionally omit this line
    
    pr_info("kmemleak_test: Module unloaded. Leak is now detectable by kmemleak.\n");
}

module_init(kmemleak_test_init);
module_exit(kmemleak_test_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swarna");
