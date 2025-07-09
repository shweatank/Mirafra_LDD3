#include <linux/module.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>      // for jiffies
#include <linux/delay.h>        // for msecs_to_jiffies

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sowmya Sree");
MODULE_DESCRIPTION("Workqueue vs Delayed Work Example");

// Normal work struct
static struct work_struct my_work;

// Delayed work struct
static struct delayed_work my_delayed_work;

// Normal work handler
static void normal_work_handler(struct work_struct *work)
{
    pr_info(">> Normal work executed immediately!\n");
}

// Delayed work handler
static void delayed_work_handler(struct work_struct *work)
{
    pr_info(">> Delayed work executed after 5 seconds!\n");
}

static int __init workqueue_example_init(void)
{
    pr_info("=== Workqueue Module Loaded ===\n");

    // Initialize and schedule normal work
    INIT_WORK(&my_work, normal_work_handler);
    schedule_work(&my_work);

    // Initialize and schedule delayed work
    INIT_DELAYED_WORK(&my_delayed_work, delayed_work_handler);
    schedule_delayed_work(&my_delayed_work, msecs_to_jiffies(5000)); // 5 seconds delay

    return 0;
}

static void __exit workqueue_example_exit(void)
{
    // Cancel work safely (if still running)
    cancel_work_sync(&my_work);
    cancel_delayed_work_sync(&my_delayed_work);

    pr_info("=== Workqueue Module Unloaded ===\n");
}

module_init(workqueue_example_init);
module_exit(workqueue_example_exit);

