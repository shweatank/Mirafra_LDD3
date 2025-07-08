
#include <linux/kernel.h>     
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define KEYBOARD_IRQ    1
#define KBD_DATA_PORT   0x60

// IRQ handler function
irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    scancode = inb(KBD_DATA_PORT);

    printk(KERN_INFO "Keyboard IRQ: scancode = 0x%02x\n", scancode);

    return IRQ_HANDLED;
}

// Initialization function
static int __init keyboard_irq_init(void)
{
    int result;

    printk(KERN_INFO "Loading custom keyboard IRQ handler...\n");

    result = request_irq(KEYBOARD_IRQ,                  // IRQ number
                         keyboard_irq_handler,          // Handler function
                         IRQF_SHARED,                   // Shared interrupt
                         "keyboard_irq_handler",        // Name shown in /proc/interrupts
                         (void *)keyboard_irq_handler); // dev_id must be unique

    if (result) {
        printk(KERN_ERR "keyboard_irq: Cannot register IRQ %d\n", KEYBOARD_IRQ);
        return result;
    }

    printk(KERN_INFO "keyboard_irq: Handler registered successfully.\n");
    return 0;
}

// Cleanup function
static void __exit keyboard_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)keyboard_irq_handler);
    printk(KERN_INFO "keyboard_irq: Module unloaded.\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple Keyboard IRQ Handler");

