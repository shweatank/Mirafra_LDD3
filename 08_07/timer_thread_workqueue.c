#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rev");
MODULE_DESCRIPTION("Kernel Thread + Timer + Workqueue Example");

static struct task_struct *my_thread;
static struct timer_list my_timer;
static struct workqueue_struct *my_wq;

struct my_work_data {
    struct work_struct work;
    int count;
};

static int global_count = 0;

static void my_work_handler(struct work_struct *work)
{
    struct my_work_data *data = container_of(work, struct my_work_data, work);
    pr_info("workqueue: Doing work! Count = %d\n", data->count);
    kfree(data);
}

static void my_timer_callback(struct timer_list *t)
{
    struct my_work_data *data;

    data = kmalloc(sizeof(*data), GFP_ATOMIC);
    if (!data)
        return;

    INIT_WORK(&data->work, my_work_handler);
    data->count = global_count++;

    queue_work(my_wq, &data->work);

    // Re-arm the timer for next interval (2 seconds)
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
}

static int my_thread_fn(void *data)
{
    pr_info("kthread: Started with PID %d\n", current->pid);

    // Setup timer
    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    while (!kthread_should_stop()) {
        ssleep(1); // Simulate doing other work
    }

    pr_info("kthread: Stopping...\n");
    del_timer_sync(&my_timer);
    return 0;
}

static int __init my_module_init(void)
{
    pr_info("Module loaded: Starting components...\n");

    // Create workqueue
    my_wq = alloc_workqueue("my_wq", WQ_UNBOUND, 0);
    if (!my_wq)
        return -ENOMEM;

    // Start the kernel thread
    my_thread = kthread_run(my_thread_fn, NULL, "my_kthread");
    if (IS_ERR(my_thread)) {
        destroy_workqueue(my_wq);
        return PTR_ERR(my_thread);
    }

    return 0;
}

static void __exit my_module_exit(void)
{
    if (my_thread)
        kthread_stop(my_thread);

    flush_workqueue(my_wq);
    destroy_workqueue(my_wq);

    pr_info("Module unloaded: All components stopped.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

