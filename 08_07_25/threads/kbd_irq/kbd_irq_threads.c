#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sowmya's Assistant");
MODULE_DESCRIPTION("Keyboard IRQ with threaded handler");

#define KBD_IRQ 1  // IRQ number for keyboard on x86

static irqreturn_t kbd_top_half(int irq, void *dev_id)
{
    // Optional top-half
    printk(KERN_INFO "kbd: top-half handler (IRQ: %d)\n", irq);
    return IRQ_WAKE_THREAD;  // Let thread handler run
}

static irqreturn_t kbd_thread_fn(int irq, void *dev_id)
{
    unsigned char scancode;

    // Read scancode from keyboard data port (0x60)
    scancode = inb(0x60);

    printk(KERN_INFO "kbd: threaded handler, scancode: 0x%x\n", scancode);

    // You can sleep here if needed, like:
    // msleep(10);

    return IRQ_HANDLED;
}

static int __init kbd_irq_init(void)
{
    int ret;

    ret = request_threaded_irq(KBD_IRQ, kbd_top_half, kbd_thread_fn,
                               IRQF_SHARED, "kbd_irq_threaded", (void *)(kbd_thread_fn));
    if (ret) {
        printk(KERN_ERR "kbd: Failed to request IRQ\n");
        return ret;
    }

    printk(KERN_INFO "kbd: Module loaded with threaded handler\n");
    return 0;
}

static void __exit kbd_irq_exit(void)
{
    free_irq(KBD_IRQ, (void *)(kbd_thread_fn));
    printk(KERN_INFO "kbd: Module unloaded\n");
}

module_init(kbd_irq_init);
module_exit(kbd_irq_exit);

