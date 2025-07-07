/*#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    // Read the scancode from keyboard data port
    scancode = inb(KBD_DATA_PORT);

    // Log the scancode to kernel log
    printk(KERN_INFO "Keyboard IRQ: scancode = 0x%02x\n", scancode);

    return IRQ_HANDLED;
}



static int __init keyboard_irq_init(void)
{
    int result;

    printk(KERN_INFO "Loading custom keyboard IRQ handler\n");

    // Request IRQ line 1 (keyboard IRQ) with shared flag
    result = request_irq(KEYBOARD_IRQ, keyboard_irq_handler,
                         IRQF_SHARED, "keyboard_irq_handler",
                         (void *)(keyboard_irq_handler));
    if (result) {
        printk(KERN_ERR "keyboard_irq: cannot register IRQ %d\n", KEYBOARD_IRQ);
        return result;
    }

    printk(KERN_INFO "keyboard_irq: IRQ handler registered successfully\n");
    return 0;
}

static void __exit keyboard_irq_exit(void)
{
    // Free the IRQ line when module is removed
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    printk(KERN_INFO "keyboard_irq: IRQ handler removed\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mamta");
MODULE_DESCRIPTION("Keyboard IRQ handler for x86 Linux");*/



#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

static int result = 0;
static unsigned char scancode = 0;

// Work structure
static struct work_struct kb_work;

// Work handler function (process context)
static void kb_work_handler(struct work_struct *work)
{
    switch(scancode) {
        case 0x1E:  // 'A'
            result += 1;
            printk(KERN_INFO "Operation: ADD 1, Result = %d\n", result);
            break;

        case 0x30:  // 'B'
            result -= 1;
            printk(KERN_INFO "Operation: SUB 1, Result = %d\n", result);
            break;

        default:
            // Ignore other keys or extend here
            break;
    }
}

// IRQ handler (interrupt context)
irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char sc;

    sc = inb(KBD_DATA_PORT);

    if (!(sc & 0x80)) {  // key press only
        scancode = sc;   // save scancode
        schedule_work(&kb_work);  // defer work to process context
    }

    return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
    int ret;

    printk(KERN_INFO "Loading keyboard IRQ handler with workqueue\n");

    INIT_WORK(&kb_work, kb_work_handler);

    ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler,
                      IRQF_SHARED, "keyboard_irq_handler",
                      (void *)(keyboard_irq_handler));
    if (ret) {
        printk(KERN_ERR "Cannot register IRQ %d\n", KEYBOARD_IRQ);
        return ret;
    }

    printk(KERN_INFO "Keyboard IRQ handler registered\n");
    return 0;
}

static void __exit keyboard_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    cancel_work_sync(&kb_work);
    printk(KERN_INFO "Keyboard IRQ handler removed\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mamta");
MODULE_DESCRIPTION("Keyboard IRQ handler with deferred work");

