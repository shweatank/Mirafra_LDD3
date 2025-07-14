#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rev");
MODULE_DESCRIPTION("Kernel Thread + Timer + Waitqueue Example");

static struct task_struct *my_thread;
static struct timer_list my_timer;
static wait_queue_head_t my_wq;
static int wake_condition = 0;   // Flag to control wakeup
static int counter = 0;

// --- Timer callback ---
static void my_timer_callback(struct timer_list *t)
{
    pr_info("timer: Triggered! Waking up thread\n");

    wake_condition = 1;
    wake_up_interruptible(&my_wq);

    // Re-arm the timer for another 2 seconds
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
}

// --- Thread function ---
static int my_thread_fn(void *data)
{
    pr_info("kthread: Started with PID %d\n", current->pid);

    while (!kthread_should_stop()) {
        pr_info("kthread: Sleeping...\n");

        wait_event_interruptible(my_wq, wake_condition || kthread_should_stop());

        if (kthread_should_stop())
            break;

        pr_info("kthread: Woken up! Count = %d\n", counter++);
        wake_condition = 0;
    }

    pr_info("kthread: Exiting\n");
    return 0;
}

// --- Module Init ---
static int __init my_module_init(void)
{
    pr_info("Module init: Starting components...\n");

    init_waitqueue_head(&my_wq);

    // Initialize and start the timer
    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    // Start the kernel thread
    my_thread = kthread_run(my_thread_fn, NULL, "my_kthread");
    if (IS_ERR(my_thread)) {
        pr_err("Failed to start kernel thread\n");
        del_timer_sync(&my_timer);
        return PTR_ERR(my_thread);
    }

    return 0;
}

// --- Module Exit ---
static void __exit my_module_exit(void)
{
    pr_info("Module exit: Cleaning up...\n");

    if (my_thread)
        kthread_stop(my_thread);  // This also wakes up the thread if sleeping

    del_timer_sync(&my_timer);

    pr_info("Module exit: Done.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

