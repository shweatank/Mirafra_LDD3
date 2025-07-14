#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rev");
MODULE_DESCRIPTION("Workqueue-based Calculator Triggered by Keyboard IRQ");

// --- Calculation data ---
struct calc_data {
    int a;
    int b;
    char op;     // '+', '-', '*', '/'
    int result;
};

static struct calc_data my_calc = {
    .a = 15,
    .b = 3,
    .op = '*',
    .result = 0
};

// --- Workqueue + Delayed Work ---
static struct workqueue_struct *wq;

static void calc_work_func(struct work_struct *work);

// Declare delayed work
static DECLARE_DELAYED_WORK(work, calc_work_func);

// --- Work Handler ---
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

// --- IRQ Handler ---
static irqreturn_t keyboard_handler(int irq, void *dev_id)
{
    u8 sc = inb(KBD_DATA_PORT); // Read scan code

    pr_info("IRQ: Keyboard interrupt received. scancode=0x%x\n", sc);

    // Schedule work immediately
    queue_delayed_work(wq, &work, 0);

    return IRQ_HANDLED;
}

// --- Module Init ---
static int __init workqueue_calc_init(void)
{
    pr_info("Workqueue Calculator Module Loaded\n");

    // Create workqueue
    wq = alloc_workqueue("wq_calc", WQ_UNBOUND, 0);
    if (!wq)
        return -ENOMEM;

    INIT_DELAYED_WORK(&work, calc_work_func);

    // Request IRQ
    if (request_irq(KEYBOARD_IRQ, keyboard_handler, IRQF_SHARED,
                    "workqueue_calc_irq", (void *)keyboard_handler)) {
        pr_err("Failed to register IRQ %d\n", KEYBOARD_IRQ);
        destroy_workqueue(wq);
        return -EIO;
    }

    return 0;
}

// --- Module Exit ---
static void __exit workqueue_calc_exit(void)
{
    pr_info("Workqueue Calculator Module Unloading\n");

    // Cancel any pending work
    cancel_delayed_work_sync(&work);

    // Free IRQ
    free_irq(KEYBOARD_IRQ, (void *)keyboard_handler);

    // Destroy workqueue
    if (wq)
        destroy_workqueue(wq);
}

module_init(workqueue_calc_init);
module_exit(workqueue_calc_exit);

/*#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/moduleparam.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rev");
MODULE_DESCRIPTION("Workqueue-based Calculator Triggered by Keyboard IRQ");

// --- User Parameters ---
static int a = 0;
static int b = 0;
static char op = '+';

module_param(a, int, 0644);
MODULE_PARM_DESC(a, "Operand A");

module_param(b, int, 0644);
MODULE_PARM_DESC(b, "Operand B");

module_param(op, byte, 0644);  // 'byte' type for char
MODULE_PARM_DESC(op, "Operator: + - * /");

// --- Calculation data ---
struct calc_data {
    int a;
    int b;
    char op;     // '+', '-', '*', '/'
    int result;
};

static struct calc_data my_calc;

// --- Workqueue + Delayed Work ---
static struct workqueue_struct *wq;

static void calc_work_func(struct work_struct *work);

// Declare delayed work
static DECLARE_DELAYED_WORK(work, calc_work_func);

// --- Work Handler ---
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

// --- IRQ Handler ---
static irqreturn_t keyboard_handler(int irq, void *dev_id)
{
    u8 sc = inb(KBD_DATA_PORT); // Read scan code

    pr_info("IRQ: Keyboard interrupt received. scancode=0x%x\n", sc);

    // Schedule work immediately
    queue_delayed_work(wq, &work, 0);

    return IRQ_HANDLED;
}

// --- Module Init ---
static int __init workqueue_calc_init(void)
{
    pr_info("Workqueue Calculator Module Loaded\n");

    // Initialize calc_data from module parameters
    my_calc.a = a;
    my_calc.b = b;
    my_calc.op = op;

    // Create workqueue
    wq = alloc_workqueue("wq_calc", WQ_UNBOUND, 0);
    if (!wq)
        return -ENOMEM;

    INIT_DELAYED_WORK(&work, calc_work_func);

    // Request IRQ
    if (request_irq(KEYBOARD_IRQ, keyboard_handler, IRQF_SHARED,
                    "workqueue_calc_irq", (void *)keyboard_handler)) {
        pr_err("Failed to register IRQ %d\n", KEYBOARD_IRQ);
        destroy_workqueue(wq);
        return -EIO;
    }

    return 0;
}

// --- Module Exit ---
static void __exit workqueue_calc_exit(void)
{
    pr_info("Workqueue Calculator Module Unloading\n");

    // Cancel any pending work
    cancel_delayed_work_sync(&work);

    // Free IRQ
    free_irq(KEYBOARD_IRQ, (void *)keyboard_handler);

    // Destroy workqueue
    if (wq)
        destroy_workqueue(wq);
}

module_init(workqueue_calc_init);
module_exit(workqueue_calc_exit);
*/
