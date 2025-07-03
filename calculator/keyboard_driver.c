#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <linux/slab.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mamta");
MODULE_DESCRIPTION("Custom Keyboard IRQ Handler for x86 Linux");

static bool key_pressed[256] = { false };  // To prevent repeated logs for held keys

irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);
    unsigned char clean_code = scancode & 0x7F;

    if (!(scancode & 0x80)) {
        // Only log key press (make code)
        if (!key_pressed[clean_code]) {
            key_pressed[clean_code] = true;
            printk(KERN_INFO "Keyboard IRQ: key press scancode = 0x%02x\n", clean_code);
        }
    } else {
        // Key release (break code)
        key_pressed[clean_code] = false;
    }

    return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
    int result;
    printk(KERN_INFO "Loading custom keyboard IRQ handler...\n");

    result = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED,
                         "keyboard_irq_handler", (void *)(keyboard_irq_handler));

    if (result) {
        printk(KERN_ERR "keyboard_irq: Cannot register IRQ %d\n", KEYBOARD_IRQ);
        return result;
    }

    printk(KERN_INFO "keyboard_irq: IRQ handler registered successfully\n");
    return 0;
}

static void __exit keyboard_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    printk(KERN_INFO "keyboard_irq: IRQ handler removed\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

