#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/smp.h>
#include <linux/input.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hemanth");
MODULE_DESCRIPTION("Tasklet-based Arithmetic Operations on Key Press");

// Tasklet declaration
static struct tasklet_struct math_tasklet;

// Operation type
typedef enum {
    OP_NONE,
    OP_ADD,
    OP_SUB,
    OP_MUL
} operation_t;

static volatile operation_t current_op = OP_NONE;

// Sample input values for demonstration
static int a = 5, b = 3;

// Tasklet function
static void math_tasklet_func(struct tasklet_struct *tasklet)
{
    int result = 0;

    switch (current_op) {
    case OP_ADD:
        result = a + b;
        pr_info("Tasklet: ADD %d + %d = %d\n", a, b, result);
        break;
    case OP_SUB:
        result = a - b;
        pr_info("Tasklet: SUB %d - %d = %d\n", a, b, result);
        break;
    case OP_MUL:
        result = a * b;
        pr_info("Tasklet: MUL %d * %d = %d\n", a, b, result);
        break;
    default:
        pr_info("Tasklet: No operation set\n");
        break;
    }

    // Reset operation
    current_op = OP_NONE;
}

// Keyboard event notifier function
static int keyboard_event_notify(struct notifier_block *nblock,
                                 unsigned long code, void *_param)
{
    struct keyboard_notifier_param *param = _param;

    // Only handle key press (not release)
    if (code == KBD_KEYSYM && param->down) {
        char key = param->value;

        switch (key) {
        case 'a':
            current_op = OP_ADD;
            tasklet_schedule(&math_tasklet);
            break;
        case 's':
            current_op = OP_SUB;
            tasklet_schedule(&math_tasklet);
            break;
        case 'm':
            current_op = OP_MUL;
            tasklet_schedule(&math_tasklet);
            break;
        default:
            break;
        }
    }

    return NOTIFY_OK;
}

// Notifier block setup
static struct notifier_block kb_notifier = {
    .notifier_call = keyboard_event_notify
};

// Module init
static int __init tasklet_keyboard_init(void)
{
    pr_info("Tasklet + Keyboard Module Loaded\n");

    // Setup tasklet
    tasklet_setup(&math_tasklet, math_tasklet_func);

    // Register keyboard notifier
    register_keyboard_notifier(&kb_notifier);

    return 0;
}

// Module exit
static void __exit tasklet_keyboard_exit(void)
{
    pr_info("Tasklet + Keyboard Module Unloaded\n");

    // Unregister keyboard notifier
    unregister_keyboard_notifier(&kb_notifier);

    // Kill tasklet
    tasklet_kill(&math_tasklet);
}

module_init(tasklet_keyboard_init);
module_exit(tasklet_keyboard_exit);

