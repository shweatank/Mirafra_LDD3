#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>

#define KBD_IRQ 1  // IRQ number for keyboard (usually 1 for PS/2)

static struct tasklet_struct my_tasklet;

// Tasklet handler function
void tasklet_handler(unsigned long data)
{
    pr_info("Tasklet: Keyboard interrupt handled in bottom half\n");
}

// IRQ Handler (Top-half)
static irqreturn_t irq_handler(int irq, void *dev_id)
{
    pr_info("IRQ: Keyboard interrupt occurred (Top-half)\n");

    // Schedule tasklet
    tasklet_schedule(&my_tasklet);

    return IRQ_HANDLED;
}

static int __init my_init(void)
{
    pr_info("Loading keyboard IRQ tasklet module\n");

    // Initialize tasklet
    tasklet_init(&my_tasklet, tasklet_handler, 0);

    // Request IRQ for keyboard
    if (request_irq(KBD_IRQ, irq_handler, IRQF_SHARED, "kbd_tasklet", (void *)(irq_handler))) {
        pr_err("Cannot register IRQ %d\n", KBD_IRQ);
        return -EIO;
    }

    pr_info("Module loaded successfully\n");
    return 0;
}

static void __exit my_exit(void)
{
    pr_info("Unloading module\n");

    // Free IRQ
    free_irq(KBD_IRQ, (void *)(irq_handler));

    // Kill tasklet
    tasklet_kill(&my_tasklet);

    pr_info("Module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pritesh");
MODULE_DESCRIPTION("Keyboard Interrupt Tasklet Example");

