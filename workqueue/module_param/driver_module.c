#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adinarayana");
MODULE_DESCRIPTION("Calculator with operand choice using Workqueue every 5 seconds");

static struct timer_list my_timer;
static struct workqueue_struct *my_wq;

// --------- Module Parameters -----------
static int a = 20;
static int b = 5;
static char *op = "add";  // Default operation

module_param(a, int, 0444);
MODULE_PARM_DESC(a, "First operand");

module_param(b, int, 0444);
MODULE_PARM_DESC(b, "Second operand");

module_param(op, charp, 0444);
MODULE_PARM_DESC(op, "Operation: add, sub, mul, div");
// --------------------------------------

static void calc_work_func(struct work_struct *work);
static DECLARE_WORK(my_work, calc_work_func);

// Timer handler: queue the work
static void my_timer_handler(struct timer_list *t)
{
    if (my_wq)
        queue_work(my_wq, &my_work);

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
}

// Workqueue function
static void calc_work_func(struct work_struct *work)
{
    if (strcmp(op, "add") == 0) {
        pr_info("ADD: %d + %d = %d\n", a, b, a + b);
    } else if (strcmp(op, "sub") == 0) {
        pr_info("SUB: %d - %d = %d\n", a, b, a - b);
    } else if (strcmp(op, "mul") == 0) {
        pr_info("MUL: %d * %d = %d\n", a, b, a * b);
    } else if (strcmp(op, "div") == 0) {
        if (b != 0)
            pr_info("DIV: %d / %d = %d\n", a, b, a / b);
        else
            pr_err("DIV: Division by zero error!\n");
    } else {
        pr_err("Unknown operation '%s'. Use add, sub, mul, or div.\n", op);
    }
}

// Init function
static int __init calc_init(void)
{
    pr_info("Calculator module loaded with a=%d, b=%d, op=%s\n", a, b, op);

    my_wq = create_singlethread_workqueue("calc_wq");
    if (!my_wq)
        return -ENOMEM;

    timer_setup(&my_timer, my_timer_handler, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));  // start after 5s

    return 0;
}

// Exit function
static void __exit calc_exit(void)
{
    del_timer_sync(&my_timer);

    if (my_wq) {
        flush_workqueue(my_wq);
        destroy_workqueue(my_wq);
    }

    pr_info("Calculator module unloaded.\n");
}

module_init(calc_init);
module_exit(calc_exit);

