#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/smp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Calculator using Workqueue on x86");

// --- Calculator Data ---
struct calc_data {
    int a;
    int b;
    char op; // '+', '-', '*', '/'
    int result;
};

static struct calc_data my_calc = {
    .a = 15,
    .b = 3,
    .op = '*',
    .result = 0
};

// Declare delayed work and workqueue
static struct delayed_work calc_work;
static struct workqueue_struct *calc_wq;
static struct timer_list my_timer;

// Work function
static void calc_work_func(struct work_struct *work)
{
    switch (my_calc.op) {
    case '+':
        my_calc.result = my_calc.a + my_calc.b;
        break;
    case '-':
        my_calc.result = my_calc.a - my_calc.b;
        break;
    case '*':
        my_calc.result = my_calc.a * my_calc.b;
        break;
    case '/':
        if (my_calc.b != 0)
            my_calc.result = my_calc.a / my_calc.b;
        else {
            pr_info("Workqueue Calculator: Division by zero!\n");
            return;
        }
        break;
    default:
        pr_info("Workqueue Calculator: Invalid operator '%c'\n", my_calc.op);
        return;
    }

    pr_info("Workqueue Calculator: %d %c %d = %d (CPU %u)\n",
            my_calc.a, my_calc.op, my_calc.b, my_calc.result, smp_processor_id());
}

// Timer handler
static void timer_handler(struct timer_list *t)
{
    pr_info("Timer: Simulated interrupt -> Queueing calculator work\n");

    queue_delayed_work(calc_wq, &calc_work, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000)); // Re-arm
}

// Init function
static int __init workqueue_calc_init(void)
{
    pr_info("Workqueue Calculator Module Loaded\n");

    // Create workqueue
    calc_wq = alloc_workqueue("calc_wq", WQ_UNBOUND, 0);

    // Initialize delayed work
    INIT_DELAYED_WORK(&calc_work, calc_work_func);

    // Set up timer
    timer_setup(&my_timer, timer_handler, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    return 0;
}

// Exit function
static void __exit workqueue_calc_exit(void)
{
    pr_info("Workqueue Calculator Module Unloading\n");

    del_timer_sync(&my_timer);
    cancel_delayed_work_sync(&calc_work);

    if (calc_wq)
        destroy_workqueue(calc_wq);
}

module_init(workqueue_calc_init);
module_exit(workqueue_calc_exit);

