#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("Calculator using timer, kthreads, and wait queue (no IRQ)");

static int a = 30, b = 6;
static int calc_flag = 0;
static int finished_threads = 0;
static DECLARE_WAIT_QUEUE_HEAD(calc_wq);
static struct task_struct *add_thread, *sub_thread, *mul_thread, *div_thread;
static struct timer_list my_timer;
static void my_timer_handler(struct timer_list *t)
{
    pr_info("calc_timer: Timer fired, waking up calculator threads\n");

    calc_flag = 1;
    finished_threads = 0;
    wake_up_interruptible(&calc_wq);

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
}

static int calculator_thread_fn(void *arg)
{
    char *name = (char *)arg;
    int result;
    while (!kthread_should_stop()) 
    {
        wait_event_interruptible(calc_wq, calc_flag || kthread_should_stop());
        if (kthread_should_stop())
            break;

        if (strcmp(name, "add") == 0)
            result = a + b;
        else if (strcmp(name, "sub") == 0)
            result = a - b;
        else if (strcmp(name, "mul") == 0)
            result = a * b;
        else if (strcmp(name, "div") == 0)
            result = (b != 0) ? a / b : 0;
        else
            result = -1;

        pr_info("calc_timer: [%s thread] result = %d\n", name, result);

        msleep(100);

        finished_threads++;

        if (finished_threads == 4) 
	{
            calc_flag = 0;
            finished_threads = 0;
	    wake_up_interruptible(&calc_wq);
        }
	wait_event_interruptible(calc_wq, calc_flag == 0 || kthread_should_stop());

    }

    return 0;
}

static int __init calc_timer_module_init(void)
{
    pr_info("calc_timer: Module loading\n");

    timer_setup(&my_timer, my_timer_handler, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

    add_thread = kthread_run(calculator_thread_fn, "add", "add_thread");
    sub_thread = kthread_run(calculator_thread_fn, "sub", "sub_thread");
    mul_thread = kthread_run(calculator_thread_fn, "mul", "mul_thread");
    div_thread = kthread_run(calculator_thread_fn, "div", "div_thread");

    if (IS_ERR(add_thread) || IS_ERR(sub_thread) ||
        IS_ERR(mul_thread) || IS_ERR(div_thread)) {
        pr_err("calc_timer: Failed to create threads\n");
        del_timer_sync(&my_timer);
        return -ENOMEM;
    }

    pr_info("calc_timer: Module loaded successfully\n");
    return 0;
}

static void __exit calc_timer_module_exit(void)
{
    pr_info("calc_timer: Module unloading\n");

    del_timer_sync(&my_timer);

    if (add_thread) kthread_stop(add_thread);
    if (sub_thread) kthread_stop(sub_thread);
    if (mul_thread) kthread_stop(mul_thread);
    if (div_thread) kthread_stop(div_thread);

    wake_up_interruptible(&calc_wq);
}

module_init(calc_timer_module_init);
module_exit(calc_timer_module_exit);

