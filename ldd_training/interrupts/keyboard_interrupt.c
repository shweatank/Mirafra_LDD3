#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#define KEYBOARD_IRQ     1       // IRQ1 for keyboard
#define KBD_DATA_PORT    0x60    // Keyboard I/O port

// Interrupt handler
irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    scancode = inb(KBD_DATA_PORT);
    printk(KERN_INFO "keyboard_irq: scancode = 0x%02x\n", scancode);

    return IRQ_HANDLED;
}

// Init function
static int __init keyboard_irq_init(void)
{
    int result;

    printk(KERN_INFO "keyboard_irq: loading custom IRQ handler...\n");

    result = request_irq(KEYBOARD_IRQ, keyboard_irq_handler,
                         IRQF_SHARED, "keyboard_irq_handler", (void *)keyboard_irq_handler);


    if (result) {
        printk(KERN_ERR "keyboard_irq: cannot register IRQ %d\n", KEYBOARD_IRQ);
        return result;
    }

    printk(KERN_INFO "keyboard_irq: IRQ handler registered successfully\n");
    return 0;
}

// Exit function
static void __exit keyboard_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)keyboard_irq_handler);
    printk(KERN_INFO "keyboard_irq: IRQ handler removed\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Keyboard IRQ handler for x86 Linux");

