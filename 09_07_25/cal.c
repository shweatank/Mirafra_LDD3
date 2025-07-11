#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Timer & Keyboard IRQ triggering kthread, tasklet, and workqueue");

#define KEYBOARD_IRQ 1

// Timer 
static struct timer_list my_timer;

//  Threads
static struct task_struct *timer_thread;
static struct task_struct *kbd_thread;

// Workqueues
static DECLARE_WORK(timer_work, NULL);
static DECLARE_WORK(kbd_work, NULL);

//Tasklets 
static void timer_tasklet_fn(unsigned long);
static void kbd_tasklet_fn(unsigned long);

DECLARE_TASKLET(timer_tasklet, timer_tasklet_fn);
DECLARE_TASKLET(kbd_tasklet, kbd_tasklet_fn);

//  Timer Interrupt Handler
static void timer_callback(struct timer_list *t) {
    pr_info("TIMER: Interrupt triggered\n");

    // kthread → Addition
    timer_thread = kthread_run([](void *data),NULL,"kbd_kthread");

    // tasklet → Subtraction
    tasklet_schedule(&timer_tasklet);

    // workqueue → Multiplication
    INIT_WORK(&timer_work, [](struct work_struct *wq)) ;
    schedule_work(&timer_work);

    // rearm the timer
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
}

// Timer Tasklet 
static void timer_tasklet_fn(unsigned long data) {
    pr_info("TIMER: Tasklet → 20 - 8 = %d\n", 20 - 8);
}

// Keyboard IRQ Handler 
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id) {
    pr_info("KBD: Interrupt triggered\n");

    // kthread → Division
kbd_thread = kthread_run([](void *data), NULL, "kbd_kthread");

    // tasklet → Modulo
    tasklet_schedule(&kbd_tasklet);

    // workqueue → Square
    INIT_WORK(&kbd_work, [](struct work_struct *wq));
    schedule_work(&kbd_work);

    return IRQ_HANDLED;
}

// Keyboard Tasklet 
static void kbd_tasklet_fn(unsigned long data) {
    pr_info("KBD: Tasklet → 23 %% 4 = %d\n", 23 % 4);
}

// Module Init 
static int __init irq_math_init(void) {
    int ret;

    pr_info("Loading module...\n");

    // Register keyboard IRQ
    ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED, "kbd_irq_math", (void *)(keyboard_irq_handler));
    if (ret) {
        pr_err("Failed to request keyboard IRQ\n");
        return ret;
    }

    // Setup and start timer
    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

    pr_info("Module loaded. Wait for timer or press any key...\n");
    return 0;
}

// Module Exit 
static void __exit irq_math_exit(void) {
    pr_info("Unloading module...\n");

    // Cleanup
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

    pr_info("Module unloaded.\n");
}

module_init(irq_math_init);
module_exit(irq_math_exit);

