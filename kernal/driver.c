#include <linux/module.h>
#include <linux/kernel.h>  // Correct spelling
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PRITESH SAWANT");
MODULE_DESCRIPTION("A simple Hello World Linux Kernel Module");

// Initialization function
static int __init hello_init(void)
{
    pr_info("Hello, Kernel!\n");
    pr_debug("Debug: Hello, Kernel!\n");
    pr_err("Error: Hello, Kernel!\n");
    pr_warn("Warning: Hello, Kernel!\n");
    pr_notice("Notice: Hello, Kernel!\n");
    return 0;
}

// Exit function
static void __exit hello_exit(void)
{
    pr_info("Goodbye, Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);

