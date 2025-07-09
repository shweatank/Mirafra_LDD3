#include<linux/.odule.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/wait.h>
#include<linux/delay.h>

MODULE_LICENSE("GPL");


static DECLARE_WAIT_QUEUE_HEAD(my_queue);
static int condition=0;

static int __init simple_waitqueue_init(void)
{
	int ret;
	printk(KERN_INFO "Wait Queue Demo: Module loaded\n");

    // Simulate waiting (condition is false)
    printk(KERN_INFO "Waiting for condition to become true...\n");

    ret = wait_event_interruptible_timeout(my_queue, condition != 0, msecs_to_jiffies(5000));

    if (ret == 0) {
        printk(KERN_INFO "Timeout occurred, condition still false.\n");
    } else {
        printk(KERN_INFO "Woke up, condition is true!\n");
    }

    // Now simulate waking
    condition = 1;
    wake_up_interruptible(&my_queue);

    return 0;
}

static void __exit simple_waitqueue_exit(void)
{
    printk(KERN_INFO "Wait Queue Demo: Module unloaded\n");
}

module_init(simple_waitqueue_init);
module_exit(simple_waitqueue_exit);
