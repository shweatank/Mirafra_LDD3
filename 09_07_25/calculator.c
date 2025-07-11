#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("Timer and Keyboard IRQ triggering kthread, tasklet, and workqueue");

#define KEYBOARD_IRQ 1

// ----------- Timer -----------
static struct timer_list my_timer;

// ----------- Threads -----------
static struct task_struct *timer_thread;
static struct task_struct *kbd_thread;

// ----------- Workqueues -----------
static DECLARE_WORK(timer_work, NULL);
static DECLARE_WORK(kbd_work, NULL);

// ----------- Tasklets -----------
static void timer_tasklet_fn(unsigned long data);
static void kbd_tasklet_fn(unsigned long data);

// Note: Use _OLD macro for newer kernels (6.8+)
/*DECLARE_TASKLET_OLD(timer_tasklet, timer_tasklet_fn, 0);
DECLARE_TASKLET_OLD(kbd_tasklet, kbd_tasklet_fn, 0);
*/
DECLARE_TASKLET_OLD(timer_tasklet, timer_tasklet_fn);     
DECLARE_TASKLET_OLD(kbd_tasklet, kbd_tasklet_fn);         

// ----------- Kthread Functions -----------

static int timer_kthread_fn(void *data)
{
    pr_info("TIMER: KThread → 10 + 5 = %d\n", 10 + 5);
    return 0;
}

static int kbd_kthread_fn(void *data)
{
    pr_info("KBD: KThread → 40 / 5 = %d\n", 40 / 5);
    return 0;
}

// ----------- Workqueue Functions -----------

static void timer_work_fn(struct work_struct *work)
{
    pr_info("TIMER: Workqueue → 6 * 7 = %d\n", 6 * 7);
}

static void kbd_work_fn(struct work_struct *work)
{
    pr_info("KBD: Workqueue → 7 ^ 2 = %d\n", 7 * 7);
}

// ----------- Tasklet Functions -----------

static void timer_tasklet_fn(unsigned long data)
{
    pr_info("TIMER: Tasklet → 20 - 8 = %d\n", 20 - 8);
}

static void kbd_tasklet_fn(unsigned long data)
{
    pr_info("KBD: Tasklet → 23 %% 4 = %d\n", 23 % 4);
}

// ----------- Timer Callback -----------
static void timer_callback(struct timer_list *t)
{
    pr_info("TIMER: Interrupt Triggered\n");

    // Kthread
    timer_thread = kthread_run(timer_kthread_fn, NULL, "timer_kthread");

    // Tasklet
    tasklet_schedule(&timer_tasklet);

    // Workqueue
    INIT_WORK(&timer_work, timer_work_fn);
    schedule_work(&timer_work);

    // Rearm the timer
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
}

// ----------- Keyboard IRQ Handler -----------
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    pr_info("KBD: Interrupt Triggered\n");

    // Kthread
    kbd_thread = kthread_run(kbd_kthread_fn, NULL, "kbd_kthread");

    // Tasklet
    tasklet_schedule(&kbd_tasklet);

    // Workqueue
    INIT_WORK(&kbd_work, kbd_work_fn);
    schedule_work(&kbd_work);

    return IRQ_HANDLED;
}

// ----------- Init Function -----------
static int __init cal_init(void)
{
    int ret;

    pr_info("Loading module: IRQ math ops\n");

    // Register keyboard IRQ
    ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED, "kbd_irq_math", (void *)(keyboard_irq_handler));
    if (ret) {
        pr_err("Failed to request keyboard IRQ\n");
        return ret;
    }

    // Setup and start timer
    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

    pr_info("Module loaded. Press a key or wait for timer...\n");
    return 0;
}

// ----------- Exit Function -----------
static void __exit cal_exit(void)
{
    pr_info("Unloading module...\n");

    del_timer_sync(&my_timer);
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));

    tasklet_kill(&timer_tasklet);
    tasklet_kill(&kbd_tasklet);

    flush_work(&timer_work);
    flush_work(&kbd_work);

    if (timer_thread)
        kthread_stop(timer_thread);
    if (kbd_thread)
        kthread_stop(kbd_thread);

    pr_info("Module unloaded successfully.\n");
}

module_init(cal_init);
module_exit(cal_exit);

