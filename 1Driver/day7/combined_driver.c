#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("Kernel Module using Software IRQ and Hardware IRQ with Tasklet and Workqueue");

// KEYBOARD ***********
#define KBD_IRQ 1

static struct task_struct *my_kthread, *my_kthread2;
static DECLARE_WAIT_QUEUE_HEAD(wq);
static int data_ready = 0;
static int data_ready2 = 0;

static int irq_dev_id = 12;

// TIMER **************
static void my_tasklet_func(struct tasklet_struct *tasklet);
static struct tasklet_struct my_tasklet;
static struct timer_list my_timer;
static struct work_struct my_work;

static int a = 5, b = 10;

// ---------- TIMER HANDLER ----------
static void my_timer_handler(struct timer_list *t)
{
    pr_info("Timer: Firing, simulating interrupt -> scheduling tasklet\n");
    tasklet_schedule(&my_tasklet);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
}

// ---------- KEYBOARD INTERRUPT HANDLER ----------
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    pr_info("irq: interrupt %d triggered\n", irq);
    schedule_work(&my_work);
    return IRQ_HANDLED;
}

// ---------- KEYBOARD KTHREAD 1----------
static int my_thread_fn(void *data)
{
    pr_info("kthread: Thread started, waiting for events\n");

    while (!kthread_should_stop()) {
        wait_event_interruptible(wq, data_ready != 0);
        if (kthread_should_stop()) break;

        pr_info("kthread: Received signal, processing...\n");
        pr_info("kthread: Values %d + %d = %d\n", a, b, a + b);
        data_ready = 0;
        ssleep(1);
        pr_info("kthread: Done processing event\n");
    }

    pr_info("kthread: Exiting\n");
    return 0;
}

// ---------- TIMER KTHREAD 2----------
static int my_thread_fn2(void *data)
{
    pr_info("kthread2: Thread started, waiting for events\n");

    while (!kthread_should_stop()) {
        wait_event_interruptible(wq, data_ready2 != 0);
        if (kthread_should_stop()) break;

        pr_info("kthread2: Received signal, processing...\n");
        pr_info("kthread2: Values %d - %d = %d\n", a, b, a - b);
        data_ready2 = 0;
        ssleep(1);
        pr_info("kthread2: Done processing event\n");
    }

    pr_info("kthread2: Exiting\n");
    return 0;
}

// ---------- WORKQUEUE HANDLER ----------
static void my_work_handler(struct work_struct *work)
{
    pr_info("workqueue: running in bottom half\n");
    data_ready = 1;
    wake_up_interruptible(&wq);
    pr_info("workqueue: woke up sleeping thread\n");
}

// ---------- TASKLET HANDLER ----------
static void my_tasklet_func(struct tasklet_struct *tasklet)
{
    pr_info("Tasklet: Executing in softirq\n");
    data_ready2 = 1;
    wake_up_interruptible(&wq);
    pr_info("Tasklet: woke up sleeping thread2\n");
}

// ---------- MODULE INIT ----------
static int __init my_module_init(void)
{
    int ret;

    pr_info("module: Initializing module\n");

    INIT_WORK(&my_work, my_work_handler);

    my_kthread = kthread_run(my_thread_fn, NULL, "my_kthread");
    if (IS_ERR(my_kthread)) {
        pr_err("module: Failed to create kernel thread\n");
        return PTR_ERR(my_kthread);
    }

    my_kthread2 = kthread_run(my_thread_fn2, NULL, "my_kthread2");
    if (IS_ERR(my_kthread2)) {
        pr_err("module: Failed to create kernel thread 2\n");
        kthread_stop(my_kthread);
        return PTR_ERR(my_kthread2);
    }

    ret = request_irq(KBD_IRQ, my_irq_handler, IRQF_SHARED, "my_irq_handler", &irq_dev_id);
    if (ret) {
        pr_err("module: Failed to register IRQ %d\n", KBD_IRQ);
        kthread_stop(my_kthread);
        kthread_stop(my_kthread2);
        return ret;
    }

    pr_info("module: IRQ %d registered, module loaded\n", KBD_IRQ);

    tasklet_setup(&my_tasklet, my_tasklet_func);

    timer_setup(&my_timer, my_timer_handler, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    return 0;
}

// ---------- MODULE EXIT ----------
static void __exit tasklet_cleanup_module(void)
{
    pr_info("module: Unloading\n");

    del_timer_sync(&my_timer);
    tasklet_kill(&my_tasklet);

    if (my_kthread)
        kthread_stop(my_kthread);
    if (my_kthread2)
        kthread_stop(my_kthread2);

    free_irq(KBD_IRQ, &irq_dev_id);

    flush_work(&my_work);

    pr_info("module: Module unloaded cleanly\n");
}

module_init(my_module_init);
module_exit(tasklet_cleanup_module);

