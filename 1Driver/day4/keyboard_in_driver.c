#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/interrupt.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

// Function prototypes
static int __init keyboard_irq_init(void);
static void __exit keyboard_irq_exit(void);

// Dummy example functions to call on key press
void add(void) {
    printk(KERN_INFO "ADD FUNCTION EXECUTED\n");
}

void sub(void) {
    printk(KERN_INFO "SUB FUNCTION EXECUTED\n");
}

// IRQ handler
irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    if (scancode < 0x80) {
        switch (scancode) {
            case 0x1E: // 'A'
                printk(KERN_INFO "Key A pressed - triggering add()\n");
                add();
                break;
            case 0x30: // 'B'
                printk(KERN_INFO "Key B pressed - triggering sub()\n");
                sub();
                break;
            default:
                printk(KERN_INFO "Key pressed: scancode 0x%02x\n", scancode);
                break;
        }
    }

    return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
    int result;
    printk(KERN_INFO "Loading keyboard IRQ action handler module...\n");

    result = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED, "keyboard_action_driver", (void *)(keyboard_irq_handler));
    if (result) {
        printk(KERN_ERR "keyboard_irq: Cannot register IRQ %d\n", KEYBOARD_IRQ);
        return result;
    }

    printk(KERN_INFO "keyboard_irq: IRQ handler registered!\n");
    return 0;
}

static void __exit keyboard_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    printk(KERN_INFO "keyboard_irq: IRQ handler removed\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Keyboard IRQ with Action Execution on Key Press");

