#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/input.h>
#include <linux/keyboard.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunny Usadadiya");
MODULE_DESCRIPTION("Keyboard-triggered Tasklet Example");

// Keyboard IRQ
#define KEYBOARD_IRQ 1

// Declare tasklet
static struct tasklet_struct my_tasklet;
static int operation = 0;  // 1 for add, 2 for sub

// Two numbers to work on
static int num1 = 10;
static int num2 = 5;

// Tasklet function
static void my_tasklet_func(struct tasklet_struct *tasklet)
{
    int result;
    if (operation == 1) {
        result = num1 + num2;
        pr_info("Tasklet: Addition -> %d + %d = %d\n", num1, num2, result);
    } else if (operation == 2) {
        result = num1 - num2;
        pr_info("Tasklet: Subtraction -> %d - %d = %d\n", num1, num2, result);
    } else {
        pr_info("Tasklet: No valid operation selected.\n");
    }

    operation = 0; // reset
}

// Top-half ISR for keyboard IRQ
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    scancode = inb(0x60);  // Read scancode from keyboard controller port

    // Map scancode to operation (this is basic; real maps are more complex)
    if (scancode == 0x1E) {         // 'A' key down
        operation = 1;
        pr_info("Keyboard: 'A' pressed -> scheduling tasklet for addition\n");
        tasklet_schedule(&my_tasklet);
    } else if (scancode == 0x30) {  // 'B' key down
        operation = 2;
        pr_info("Keyboard: 'B' pressed -> scheduling tasklet for subtraction\n");
        tasklet_schedule(&my_tasklet);
    }

    return IRQ_HANDLED;
}

// Module init
static int __init tasklet_keyboard_init(void)
{
    pr_info("Keyboard Tasklet Module Loaded\n");

    tasklet_setup(&my_tasklet, my_tasklet_func);

    // Request shared IRQ for keyboard
    if (request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED, "keyboard_tasklet", (void *)(keyboard_irq_handler))) {
        pr_err("Failed to register keyboard IRQ handler\n");
        return -EIO;
    }

    return 0;
}

// Module cleanup
static void __exit tasklet_keyboard_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    tasklet_kill(&my_tasklet);
    pr_info("Keyboard Tasklet Module Unloaded\n");
}

module_init(tasklet_keyboard_init);
module_exit(tasklet_keyboard_exit);

