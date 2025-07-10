#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunny Usadadiya");
MODULE_DESCRIPTION("Timer and Threaded Keyboard IRQ with Tasklet and Workqueue (Key Based)");

#define TIMER_INTERVAL_MS 3000  // Timer interval in ms
#define KEYBOARD_IRQ 1          // Keyboard IRQ number
#define KBD_DATA_PORT 0x60      // Keyboard data port
#define DEV_NAME "kbd_irq_threaded"

bool opFlag = false;

static struct timer_list my_timer;

static struct tasklet_struct tasklet_addlet;
static struct tasklet_struct tasklet_sublet;

static void tasklet_add(unsigned long data)
{
    int a = 5, b = 7;
    pr_info("[TASKLET ADD] %d + %d = %d\n", a, b, a + b);
}

static void tasklet_sub(unsigned long data)
{
    int a = 10, b = 4;
    pr_info("[TASKLET SUB] %d - %d = %d\n", a, b, a - b);
}

static void tasklet_add_wrapper(struct tasklet_struct *t)
{
    tasklet_add(0);
}

static void tasklet_sub_wrapper(struct tasklet_struct *t)
{
    tasklet_sub(0);
}


static void work_multiply(struct work_struct *work)
{
    int a = 6, b = 3;
    pr_info("[WORKQUEUE MUL] %d * %d = %d\n", a, b, a * b);
}

static void work_divide(struct work_struct *work)
{
    int a = 20, b = 5;
    if (b != 0)
        pr_info("[WORKQUEUE DIV] %d / %d = %d\n", a, b, a / b);
    else
        pr_info("[WORKQUEUE DIV] Division by zero error!\n");
}

static DECLARE_WORK(workq_mul, work_multiply);
static DECLARE_WORK(workq_div, work_divide);

static void my_timer_callback(struct timer_list *t)
{
    pr_info("[TIMER] Timer fired!\n");

    opFlag?tasklet_schedule(&tasklet_addlet):tasklet_schedule(&tasklet_sublet);//schedule_work(&workq_mul);

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));
    opFlag = !opFlag;
}


static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    return IRQ_WAKE_THREAD;  // Let the threaded handler run
}

static irqreturn_t keyboard_thread_fn(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    if (!(scancode & 0x80)) {
        switch (scancode) {
        case 0x32:  // 'M' key
            pr_info("[IRQ] M' key pressed → Multiplication\n");
            schedule_work(&workq_mul);
            break;
        case 0x20:  // 'D' key
            pr_info("[IRQ] 'D' key pressed → Division\n");
            schedule_work(&workq_div);
            break;
        default:
            break;
        }
    }

    return IRQ_HANDLED;
}


static int __init mod_init(void)
{
    int ret;

    pr_info("== Module Init ==\n");

    tasklet_setup(&tasklet_addlet, tasklet_add_wrapper);
    tasklet_setup(&tasklet_sublet, tasklet_sub_wrapper);

    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));

    ret = request_threaded_irq(KEYBOARD_IRQ,
                               keyboard_irq_handler,
                               keyboard_thread_fn,
                               IRQF_SHARED,
                               DEV_NAME,
                               (void *)(keyboard_irq_handler));

    if (ret) {
        pr_warn("[IRQ] Failed to register IRQ %d\n", KEYBOARD_IRQ);
    } else {
        pr_info("[IRQ] Threaded IRQ %d registered successfully\n", KEYBOARD_IRQ);
    }

    return 0;
}

static void __exit mod_exit(void)
{
    del_timer_sync(&my_timer);

    flush_work(&workq_mul);
    flush_work(&workq_div);

    tasklet_kill(&tasklet_addlet);
    tasklet_kill(&tasklet_sublet);

    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));

    pr_info("== Module Exit ==\n");
}

module_init(mod_init);
module_exit(mod_exit);

