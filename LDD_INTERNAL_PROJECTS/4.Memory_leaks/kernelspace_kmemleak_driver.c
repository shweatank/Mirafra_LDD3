// kmemleak_demo.c
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Example");

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

/*

# Build (in kernel build tree or with proper Makefile)
make -C /lib/modules/$(uname -r)/build M=$PWD modules

# Load module
sudo insmod kmemleak_demo.ko

# Clear old reports, then force scan and read report
sudo su -c 'echo clear > /sys/kernel/debug/kmemleak'
sudo su -c 'echo scan > /sys/kernel/debug/kmemleak'
cat /sys/kernel/debug/kmemleak


For Scaning :
sudo sh -c "echo scan > /sys/kernel/debug/kmemleak"
echo scan | sudo tee /sys/kernel/debug/kmemleak

To Check Result:
sudo cat /sys/kernel/debug/kmemleak

To Reset kmemleak:
sudo sh -c "echo clear > /sys/kernel/debug/kmemleak"
echo clear | sudo tee /sys/kernel/debug/kmemleak



For Testing:
sudo su -c 'echo clear > /sys/kernel/debug/kmemleak'
sudo su -c 'echo scan > /sys/kernel/debug/kmemleak'
sudo cat /sys/kernel/debug/kmemleak
*/
