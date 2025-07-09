#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Keyboard IRQ with threaded handler");
MODULE_VERSION("1.0");

#define KBD_IRQ 1  // Keyboard interrupt line (typically IRQ 1)

static int dev_id = 1;

// Top-half: quick acknowledgment
static irqreturn_t kb_top_handler(int irq, void *dev)
{
    pr_info("kbd_threaded: [TOP HALF] IRQ %d received\n", irq);
    return IRQ_WAKE_THREAD;  // wake up bottom half
}

// Bottom-half: actual processing
static irqreturn_t kb_thread_handler(int irq, void *dev)
{
    pr_info("kbd_threaded: [BOTTOM HALF] handling key press...\n");
    msleep(500);  // Simulate heavy work
    pr_info("kbd_threaded: [BOTTOM HALF] done processing\n");
    return IRQ_HANDLED;
}

static int __init kbd_irq_init(void)
{
    int ret;
    pr_info("kbd_threaded: Loading module...\n");

    ret = request_threaded_irq(KBD_IRQ, kb_top_handler, kb_thread_handler,
                               IRQF_SHARED, "kbd_threaded_irq", (void *)&dev_id);
    if (ret) {
        pr_err("kbd_threaded: Failed to request IRQ %d\n", KBD_IRQ);
        return ret;
    }

    pr_info("kbd_threaded: IRQ %d handler registered\n", KBD_IRQ);
    return 0;
}

static void __exit kbd_irq_exit(void)
{
    free_irq(KBD_IRQ, (void *)&dev_id);
    pr_info("kbd_threaded: Module unloaded, IRQ %d freed\n", KBD_IRQ);
}

module_init(kbd_irq_init);
module_exit(kbd_irq_exit);

