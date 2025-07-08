#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/moduleparam.h>
#include <asm/io.h>

#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snigdha");
MODULE_DESCRIPTION("Tasklet Calculator using Keyboard IRQ and Module Parameters");

static int a = 10;
static int b = 2;
static char *operator = "+";

module_param(a, int, 0644);
module_param(b, int, 0644);
module_param(operator, charp, 0644);
MODULE_PARM_DESC(a, "Operand A");
MODULE_PARM_DESC(b, "Operand B");
MODULE_PARM_DESC(operator, "Operator: +, -, *, /");

enum operation {
    OP_NONE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

struct operation_data {
    int a;
    int b;
    enum operation op;
} op_data;

static void my_tasklet_func(struct tasklet_struct *tasklet);
static struct tasklet_struct my_tasklet;

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    // Just schedule the tasklet on any key press
    pr_info("Keyboard IRQ: Key pressed (scancode: 0x%X) - triggering tasklet\n", scancode);
    tasklet_schedule(&my_tasklet);
    return IRQ_HANDLED;
}

static void my_tasklet_func(struct tasklet_struct *tasklet)
{
    int res = 0;

    switch (op_data.op) {
    case OP_ADD:
        res = op_data.a + op_data.b;
        pr_info("Tasklet (CPU %u): %d + %d = %d\n", smp_processor_id(), op_data.a, op_data.b, res);
        break;
    case OP_SUB:
        res = op_data.a - op_data.b;
        pr_info("Tasklet (CPU %u): %d - %d = %d\n", smp_processor_id(), op_data.a, op_data.b, res);
        break;
    case OP_MUL:
        res = op_data.a * op_data.b;
        pr_info("Tasklet (CPU %u): %d * %d = %d\n", smp_processor_id(), op_data.a, op_data.b, res);
        break;
    case OP_DIV:
        if (op_data.b != 0) {
            res = op_data.a / op_data.b;
            pr_info("Tasklet (CPU %u): %d / %d = %d\n", smp_processor_id(), op_data.a, op_data.b, res);
        } else {
            pr_err("Tasklet: Division by zero error\n");
        }
        break;
    default:
        pr_info("Tasklet: No valid operation selected\n");
        break;
    }
    op_data.op = OP_NONE; // Prevent repeat
}

static int __init tasklet_init_module(void)
{
    int ret;

    // Map operator string to enum
    if (operator[0] == '+')
        op_data.op = OP_ADD;
    else if (operator[0] == '-')
        op_data.op = OP_SUB;
    else if (operator[0] == '*')
        op_data.op = OP_MUL;
    else if (operator[0] == '/')
        op_data.op = OP_DIV;
    else {
        pr_err("Invalid operator: %s\n", operator);
        return -EINVAL;
    }

    op_data.a = a;
    op_data.b = b;

    pr_info("Tasklet Calculator Module Loaded: a=%d, b=%d, operator=%s\n", a, b, operator);

    tasklet_setup(&my_tasklet, my_tasklet_func);

    ret = request_irq(KBD_IRQ, keyboard_irq_handler, IRQF_SHARED, "kb_tasklet_handler", (void *)(keyboard_irq_handler));
    if (ret) {
        pr_err("Failed to register keyboard IRQ handler\n");
        return ret;
    }

    return 0;
}

static void __exit tasklet_cleanup_module(void)
{
    pr_info("Tasklet Calculator Module Unloading\n");

    tasklet_kill(&my_tasklet);
    free_irq(KBD_IRQ, (void *)(keyboard_irq_handler));

    pr_info("Tasklet Calculator Module Unloaded\n");
}

module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);

