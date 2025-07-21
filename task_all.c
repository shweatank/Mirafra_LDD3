#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priyalatha");
MODULE_DESCRIPTION("Timer and Keyboard IRQ each triggering unique threaded handlers, tasklets, and workqueues");

static struct timer_list my_timer;

// Declarations for Timer Path
static void timer_tasklet_func(struct tasklet_struct *);
static void timer_work_func(struct work_struct *);
static irqreturn_t timer_thread_fn(int irq, void *dev_id);

// Declarations for Keyboard Path
static void kb_tasklet_func(struct tasklet_struct *);
static void kb_work_func(struct work_struct *);
static irqreturn_t keyboard_top_handler(int irq, void *dev_id);
static irqreturn_t keyboard_thread_fn(int irq, void *dev_id);

// Timer Tasklet and Work
static DECLARE_TASKLET(timer_tasklet, timer_tasklet_func);
static DECLARE_WORK(timer_work, timer_work_func);

// Keyboard Tasklet and Work
static DECLARE_TASKLET(kb_tasklet, kb_tasklet_func);
static DECLARE_WORK(kb_work, kb_work_func);

// ================= Timer Path =====================
static void timer_callback(struct timer_list *timer)
{
    pr_info("[Timer] Triggered: launching threaded handler\n");
    timer_thread_fn(0, NULL);  // Simulate interrupt threading from timer
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
}

static irqreturn_t timer_thread_fn(int irq, void *dev_id)
{
    pr_info("[Timer Threaded] Multiplying 6 * 7 = %d\n", 6 * 7);
    tasklet_schedule(&timer_tasklet);
    return IRQ_HANDLED;
}

static void timer_tasklet_func(struct tasklet_struct *t)
{
    pr_info("[Timer Tasklet] Subtracting 15 - 5 = %d\n", 15 - 5);
    schedule_work(&timer_work);
}

static void timer_work_func(struct work_struct *work)
{
    pr_info("[Timer Workqueue] Dividing 100 / 4 = %d\n", 100 / 4);
}

// ================= Keyboard Path =====================
static irqreturn_t keyboard_top_handler(int irq, void *dev_id)
{
    return IRQ_WAKE_THREAD;
}

static irqreturn_t keyboard_thread_fn(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    if (!(scancode & 0x80)) {
        pr_info("[Keyboard Threaded] Adding 9 + 11 = %d\n", 9 + 11);
        tasklet_schedule(&kb_tasklet);
    }
    return IRQ_HANDLED;
}

static void kb_tasklet_func(struct tasklet_struct *t)
{
    pr_info("[Keyboard Tasklet] Modulus 17 %% 4 = %d\n", 17 % 4);
    schedule_work(&kb_work);
}

static void kb_work_func(struct work_struct *work)
{
    pr_info("[Keyboard Workqueue] Square 6^2 = %d\n", 6 * 6);
}

// ================= Init and Exit =====================
static int __init my_module_init(void)
{
    int ret;
    pr_info("Module loaded: Timer + Keyboard IRQ each with unique paths\n");

    // Start timer
    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

    // Register keyboard IRQ
    ret = request_threaded_irq(KEYBOARD_IRQ, keyboard_top_handler,
                               keyboard_thread_fn, IRQF_SHARED,
                               "keyboard_irq_threaded", (void *)&keyboard_thread_fn);

    if (ret) {
        pr_err("Failed to request keyboard IRQ\n");
        del_timer(&my_timer);
        return ret;
    }

    return 0;
}

static void __exit my_module_exit(void)
{
    del_timer_sync(&my_timer);
    tasklet_kill(&timer_tasklet);
    tasklet_kill(&kb_tasklet);
    free_irq(KEYBOARD_IRQ, (void *)&keyboard_thread_fn);
    pr_info("Module unloaded.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

