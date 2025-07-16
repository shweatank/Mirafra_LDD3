#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <asm/io.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adinarayana");
MODULE_DESCRIPTION("Calculator using Keyboard IRQ and Tasklet");

// IRQ number for keyboard (PS/2)
#define KEYBOARD_IRQ 1

static struct tasklet_struct calc_tasklet;

// Shared data between IRQ and tasklet
static char last_scancode;

// Tasklet function (bottom half)
static void calc_tasklet_func(struct tasklet_struct *t)
{
    int a = 12, b = 4;

    pr_info("Tasklet: Running on CPU %u with scancode 0x%x\n", smp_processor_id(), last_scancode);

    switch (last_scancode) {
        case 0x1E: // 'A' key (make code)
            pr_info("Addition: %d + %d = %d\n", a, b, a + b);
            break;
        case 0x1F: // 'S' key
            pr_info("Subtraction: %d - %d = %d\n", a, b, a - b);
            break;
        case 0x20: // 'D' key
            pr_info("Multiplication: %d * %d = %d\n", a, b, a * b);
            break;
        case 0x21: // 'F' key
            if (b != 0)
                pr_info("Division: %d / %d = %d\n", a, b, a / b);
            else
                pr_info("Division by zero error\n");
            break;
        default:
            pr_info("Unsupported key pressed (scancode: 0x%x)\n", last_scancode);
    }
}

// IRQ handler (top half)
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    // Read the keyboard scan code from port 0x60
    last_scancode = inb(0x60);

    // Schedule tasklet for calculation
    tasklet_schedule(&calc_tasklet);

    return IRQ_HANDLED;
}

// Module init
static int __init calc_keyboard_init(void)
{
    int ret;

    pr_info("Calculator using Keyboard IRQ + Tasklet loaded\n");

    // Setup tasklet
    tasklet_setup(&calc_tasklet, calc_tasklet_func);

    // Request IRQ 1 (keyboard)
    ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler,
                      IRQF_SHARED, "keyboard_calc_irq", (void *)(keyboard_irq_handler));

    if (ret) {
        pr_err("Failed to register keyboard IRQ handler\n");
        return ret;
    }

    return 0;
}

// Module exit
static void __exit calc_keyboard_exit(void)
{
    pr_info("Unloading Calculator Keyboard IRQ + Tasklet module\n");

    // Free IRQ and kill tasklet
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    tasklet_kill(&calc_tasklet);
}

module_init(calc_keyboard_init);
module_exit(calc_keyboard_exit);

