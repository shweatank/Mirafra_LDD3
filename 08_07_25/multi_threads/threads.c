#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/slab.h>  // for kmalloc and kfree

#define NUM_THREADS 3

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Multiple kernel threads example");

static struct task_struct *threads[NUM_THREADS];

struct my_thread_info {
    int id;
};

static int thread_fn(void *data)
{
    struct my_thread_info *info = (struct my_thread_info *)data;
    int count = 0;

    pr_info("kthread: Thread %d started (PID=%d)\n", info->id, current->pid);

    while (!kthread_should_stop()) {
        pr_info("kthread: Thread %d running... count = %d\n", info->id, count++);
        msleep(1000);  // sleep for 1 second
    }

    pr_info("kthread: Thread %d exiting\n", info->id);
    kfree(info);  // Free the memory allocated to thread info

    return 0;
}

static int __init multi_kthread_init(void)
{
    int i;
    pr_info("kthread: Module loading with %d threads\n", NUM_THREADS);

    for (i = 0; i < NUM_THREADS; i++) {
        struct my_thread_info *info;

        info = kmalloc(sizeof(*info), GFP_KERNEL);
        if (!info)
            return -ENOMEM;

        info->id = i;

        threads[i] = kthread_create(thread_fn, info, "kthread_%d", i);
        if (IS_ERR(threads[i])) {
            pr_err("kthread: Failed to create thread %d\n", i);
            kfree(info);
            threads[i] = NULL;
        } else {
            wake_up_process(threads[i]);
        }
    }

    return 0;
}

static void __exit multi_kthread_exit(void)
{
    int i;
    pr_info("kthread: Module unloading\n");

    for (i = 0; i < NUM_THREADS; i++) {
        if (threads[i]) {
            kthread_stop(threads[i]);
            pr_info("kthread: Thread %d stopped\n", i);
        }
    }

    pr_info("kthread: All threads exited\n");
}

module_init(multi_kthread_init);
module_exit(multi_kthread_exit);

