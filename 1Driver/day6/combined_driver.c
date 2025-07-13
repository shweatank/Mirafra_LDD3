#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("Kernel Module using Workqueue, Waitqueue, IRQ, Kernel Thread");

#define KBD_IRQ 1

// --- Shared state ---
static struct task_struct *my_kthread;
static DECLARE_WAIT_QUEUE_HEAD(wq);
static int data_ready = 0;

static int irq_dev_id = 1234;

// ---------- WORK STRUCT ----------
static struct work_struct my_work;

// Workqueue handler (formerly tasklet)
static void my_work_handler(struct work_struct *work)
{
    pr_info("workqueue: running in bottom half\n");

    data_ready = 1;
    wake_up_interruptible(&wq);

    pr_info("workqueue: woke up sleeping thread\n");
}

// ---------- INTERRUPT HANDLER ----------
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    pr_info("irq: interrupt %d triggered\n", irq);

    // Schedule the bottom half work
    schedule_work(&my_work);
    return IRQ_HANDLED;
}

// ---------- KERNEL THREAD ----------
static int my_thread_fn(void *data)
{
    pr_info("kthread: Thread started, waiting for events\n");

    while (!kthread_should_stop()) {
        wait_event_interruptible(wq, data_ready != 0);
        if (kthread_should_stop())
            break;

        pr_info("kthread: Received signal, processing...\n");

        // Reset flag
        data_ready = 0;

        // Simulate processing
        ssleep(1);
        pr_info("kthread: Done processing event\n");
    }

    pr_info("kthread: Exiting\n");
    return 0;
}

// ---------- INIT ----------
static int __init my_module_init(void)
{
    int ret;

    pr_info("module: Initializing module\n");

    // Initialize work
    INIT_WORK(&my_work, my_work_handler);

    // Create kernel thread
    my_kthread = kthread_run(my_thread_fn, NULL, "my_kthread");
    if (IS_ERR(my_kthread)) {
        pr_err("module: Failed to create kernel thread\n");
        return PTR_ERR(my_kthread);
    }

    // Register IRQ
    ret = request_irq(KBD_IRQ, my_irq_handler, IRQF_SHARED, "my_irq_handler", &irq_dev_id);
    if (ret) {
        pr_err("module: Failed to register IRQ %d\n", KBD_IRQ);
        kthread_stop(my_kthread);
        return ret;
    }

    pr_info("module: IRQ %d registered, module loaded\n", KBD_IRQ);
    return 0;
}

// ---------- EXIT ----------
static void __exit my_module_exit(void)
{
    pr_info("module: Unloading module\n");

    // Stop kernel thread
    if (my_kthread)
        kthread_stop(my_kthread);

    // Free IRQ
    free_irq(KBD_IRQ, &irq_dev_id);

    // Ensure pending work is done
    flush_work(&my_work);

    pr_info("module: Module unloaded cleanly\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

