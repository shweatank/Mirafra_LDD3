#include <linux/module.h>
#include <linux/kernel.h>          // Corrected spelling from 'kernal'#include 
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/smp.h>

MODULE_LICENSE("GPL");             // Corrected from 'MOUDLE_LICENSE'
MODULE_AUTHOR("Techdhaba");
MODULE_DESCRIPTION("Updated Tasklet example for x86 Linux kernel 5.10+"); // Fixed spelling

static void my_tasklet_func(struct tasklet_struct *tasklet);

// Declare tasklet and timer structures
static struct tasklet_struct my_tasklet;
static struct timer_list my_timer;

static void my_tasklet_func(struct tasklet_struct *tasklet)
{
    pr_info("Tasklet: Executing in softirq context on CPU %u\n", smp_processor_id());
}

static void my_timer_handler(struct timer_list *t)
{
    pr_info("Timer: firing; simulating interrupt -> scheduling tasklet\n");

    tasklet_schedule(&my_tasklet);

    // Restart the timer after 2000 milliseconds
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000)); // Fixed typo: msec_to jiffeis
}

static int __init tasklet_init_module(void)
{
    pr_info("Tasklet Module Loaded\n");

    // Setup tasklet
    tasklet_setup(&my_tasklet, my_tasklet_func);

    // Setup timer
    timer_setup(&my_timer, my_timer_handler, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    return 0;
}

static void __exit tasklet_cleanup_module(void)
{
    pr_info("Tasklet Module Unloading\n");

    del_timer_sync(&my_timer);
    tasklet_kill(&my_tasklet);
}

module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);



