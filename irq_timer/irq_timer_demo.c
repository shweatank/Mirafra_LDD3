#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

#define KBD_IRQ             1
#define TIMER_PERIOD_MS     1000

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Timer + keyboard IRQ demo with tasklet & work‑queue");

static const int A = 10, B = 5;

static void calc_add(void) { pr_info("ADD : %d + %d = %d\n", A, B, A + B); }
static void calc_sub(void) { pr_info("SUB : %d - %d = %d\n", A, B, A - B); }
static void calc_mul(void) { pr_info("MUL : %d * %d = %d\n", A, B, A * B); }
static void calc_div(void)
{
        if (B)
                pr_info("DIV : %d / %d = %d\n", A, B, A / B);
        else
                pr_info("DIV : division by zero!\n");
}

static irqreturn_t kbd_top(int irq, void *dev);
static irqreturn_t kbd_thread(int irq, void *dev);
static void timer_cb(struct timer_list *);
static void tasklet_fn(struct tasklet_struct *);
static void work_fn(struct work_struct *);

static int irq_device_id = 1;
static struct timer_list demo_timer;
static struct tasklet_struct demo_tasklet;
static struct workqueue_struct *demo_wq;
static struct work_struct demo_work;

static irqreturn_t kbd_top(int irq, void *dev)
{
        return IRQ_WAKE_THREAD;
}

static irqreturn_t kbd_thread(int irq, void *dev)
{
        calc_add();
        tasklet_schedule(&demo_tasklet);
        queue_work(demo_wq, &demo_work);
        return IRQ_HANDLED;
}

static void timer_cb(struct timer_list *t)
{
        calc_div();
        tasklet_schedule(&demo_tasklet);
        queue_work(demo_wq, &demo_work);
        mod_timer(&demo_timer, jiffies + msecs_to_jiffies(TIMER_PERIOD_MS));
}

static void tasklet_fn(struct tasklet_struct *t)
{
        calc_sub();
}

static void work_fn(struct work_struct *w)
{
        calc_mul();
}

static int __init demo_init(void)
{
        int ret;

        ret = request_threaded_irq(KBD_IRQ, kbd_top, kbd_thread,
                                   IRQF_SHARED, "kbd_irq_threaded_demo",
                                   &irq_device_id);
        if (ret)
                return ret;

        tasklet_setup(&demo_tasklet, tasklet_fn);

        demo_wq = alloc_workqueue("demo_wq", WQ_UNBOUND | WQ_MEM_RECLAIM, 0);
        if (!demo_wq) {
                free_irq(KBD_IRQ, &irq_device_id);
                return -ENOMEM;
        }
        INIT_WORK(&demo_work, work_fn);

        timer_setup(&demo_timer, timer_cb, 0);
        mod_timer(&demo_timer, jiffies + msecs_to_jiffies(TIMER_PERIOD_MS));

        pr_info("irq_timer_demo_fixed loaded\n");
        return 0;
}

static void __exit demo_exit(void)
{
        del_timer_sync(&demo_timer);
        flush_workqueue(demo_wq);
        destroy_workqueue(demo_wq);
        tasklet_kill(&demo_tasklet);
        free_irq(KBD_IRQ, &irq_device_id);
        pr_info("irq_timer_demo_fixed unloaded\n");
}

module_init(demo_init);
module_exit(demo_exit);

