#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Timer + Keyboard + Threads + Tasklet + Workqueue Demo");

static int op1 = 50, op2 = 20;
static struct timer_list my_timer;

static void tasklet_func(struct tasklet_struct *t);  // updated signature for kernel 6.8+
DECLARE_TASKLET(my_tasklet, tasklet_func);

static void work_func(struct work_struct *work);
static DECLARE_WORK(my_work, work_func);

// ✅ Renamed to avoid conflict with existing IRQ_NONE in <linux/irqreturn.h>
enum my_irq_source {
    MY_IRQ_NONE = 0,
    MY_IRQ_TIMER = 1,
    MY_IRQ_KEYBOARD = 2
};

static irqreturn_t kb_irq_handler(int irq, void *dev_id);
static int calc_thread_fn(void *data);
static void timer_func(struct timer_list *t);

static void timer_func(struct timer_list *t)
{
    printk(KERN_INFO "[TIMER] Timer fired. Launching thread...\n");
    kthread_run(calc_thread_fn, (void *)(long)MY_IRQ_TIMER, "calc_timer_thread");
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
}

static irqreturn_t kb_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(0x60);
    if (scancode == 0x02)  // Key '1'
    {
        printk(KERN_INFO "[KEYBOARD] '1' pressed. Launching thread...\n");
        kthread_run(calc_thread_fn, (void *)(long)MY_IRQ_KEYBOARD, "calc_kb_thread");
    }
    return IRQ_HANDLED;
}

static int calc_thread_fn(void *data)
{
    int src = (int)(long)data;

    if (src == MY_IRQ_TIMER) {
        int result = op1 + op2;
        printk(KERN_INFO "[ADD] %d + %d = %d\n", op1, op2, result);
    } else if (src == MY_IRQ_KEYBOARD) {
        int result = op1 - op2;
        printk(KERN_INFO "[SUB] %d - %d = %d\n", op1, op2, result);
    }

    tasklet_schedule(&my_tasklet);
    schedule_work(&my_work);

    return 0;
}

// ✅ Kernel 6.8+ requires struct tasklet_struct * as argument
static void tasklet_func(struct tasklet_struct *t)
{
    int result = op1 * op2;
    printk(KERN_INFO "[TASKLET-MUL] %d * %d = %d\n", op1, op2, result);
}

static void work_func(struct work_struct *work)
{
    if (op2 != 0) {
        int result = op1 / op2;
        printk(KERN_INFO "[WORKQUEUE-DIV] %d / %d = %d\n", op1, op2, result);
    } else {
        printk(KERN_ERR "[WORKQUEUE-DIV] Division by zero!\n");
    }
}

static int __init driver_init(void)
{
    printk(KERN_INFO "Module init: starting module\n");

    if (request_irq(1, kb_irq_handler, IRQF_SHARED, "kb_irq_handler", (void *)(kb_irq_handler))) {
        printk(KERN_ERR "Cannot register keyboard IRQ\n");
        return -EIO;
    }

    timer_setup(&my_timer, timer_func, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

    printk(KERN_INFO "Module loaded successfully\n");
    return 0;
}

static void __exit driver_exit(void)
{
    del_timer_sync(&my_timer);
    tasklet_kill(&my_tasklet);
    cancel_work_sync(&my_work);
    free_irq(1, (void *)(kb_irq_handler));
    printk(KERN_INFO "Module exited cleanly\n");
}

module_init(driver_init);
module_exit(driver_exit);

