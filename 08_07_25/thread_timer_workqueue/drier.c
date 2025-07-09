#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/wait.h>

#define FAKE_IRQ 19  // This is unused, just symbolic

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kusuma");
MODULE_DESCRIPTION("Sample driver using Threaded IRQ, Workqueue, Kernel Thread, and Timer");

static struct workqueue_struct *my_wq;
static DECLARE_DELAYED_WORK(my_work, NULL);
static struct task_struct *my_thread;
static DECLARE_WAIT_QUEUE_HEAD(my_wait);
static bool data_ready = false;

static struct timer_list my_timer;

// Timer callback
void my_timer_callback(struct timer_list *t)
{
    pr_info("TIMER: Timer fired at jiffies = %lu\n", jiffies);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
}

// Workqueue function
void my_work_func(struct work_struct *work)
{
    pr_info("WORKQUEUE: Workqueue executing...\n");
    data_ready = true;
    wake_up_interruptible(&my_wait);
}

// Kernel thread function
int my_thread_fn(void *data)
{
    while (!kthread_should_stop()) {
        wait_event_interruptible(my_wait, data_ready || kthread_should_stop());

        if (data_ready) {
            pr_info("THREAD: Woken up! Processing data...\n");
            data_ready = false;
        }
    }
    return 0;
}

// Simulated IRQ handler
irqreturn_t my_threaded_irq_handler(int irq, void *dev_id)
{
    pr_info("INTERRUPT: Threaded IRQ Handler invoked!\n");
    queue_delayed_work(my_wq, &my_work, msecs_to_jiffies(1000)); // Delay 1s
    return IRQ_HANDLED;
}

static int __init sample_init(void)
{
    int ret;

    pr_info("Module loaded.\n");

    // Create workqueue
    my_wq = create_singlethread_workqueue("my_wq");
    if (!my_wq) {
        pr_err("Failed to create workqueue\n");
        return -ENOMEM;
    }

    INIT_DELAYED_WORK(&my_work, my_work_func);

    // Start kernel thread
    my_thread = kthread_run(my_thread_fn, NULL, "my_kthread");
    if (IS_ERR(my_thread)) {
        destroy_workqueue(my_wq);
        return PTR_ERR(my_thread);
    }

    // Setup timer
    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

    // 🔥 Simulate IRQ trigger manually for testing
    my_threaded_irq_handler(FAKE_IRQ, NULL);

    pr_info("Initialization complete.\n");
    return 0;
}

static void __exit sample_exit(void)
{
    del_timer_sync(&my_timer);
    kthread_stop(my_thread);
    cancel_delayed_work_sync(&my_work);
    destroy_workqueue(my_wq);
    pr_info("Module unloaded.\n");
}

module_init(sample_init);
module_exit(sample_exit);

