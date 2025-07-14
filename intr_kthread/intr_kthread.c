/*
 *
 * Demonstration of handling the keyboard interrupt (IRQ 1) with a
 *      – “top‑half” hard‑IRQ handler (cannot sleep) and
 *      – “bottom‑half” threaded handler (may sleep).
 *
 * Build:
 *      make -C /lib/modules/$(uname -r)/build M=$(PWD) modules
 * Load:
 *      sudo insmod kbd_threaded_irq.ko
 * Unload:
 *      sudo rmmod kbd_threaded_irq
 */

#include <linux/kernel.h>     /* pr_info(), pr_err()                                     */
#include <linux/module.h>     /* Needed for all loadable kernel modules                  */
#include <linux/interrupt.h>  /* request_threaded_irq(), free_irq(), irqreturn_t         */
#include <linux/init.h>       /* module_init(), module_exit()                            */
#include <linux/delay.h>      /* msleep() for simulating long work in the thread handler */

MODULE_LICENSE("GPL");            /* Keeps the kernel “untainted”                        */
MODULE_AUTHOR("Techdhaba");
MODULE_DESCRIPTION("Keyboard IRQ with threaded handler (top/bottom half)");

/* -------------------------------------------------------------------------- */
/*                              CONFIGURATION                                 */
/* -------------------------------------------------------------------------- */

#define KBD_IRQ 1                   /* On x86 PCs, the keyboard controller = IRQ 1      */

/* A unique cookie used to identify *our* handler when the line is shared.
 * It can be any non‑NULL address; we just use the address of this integer.    */
static int dev_id = 1;

/* -------------------------------------------------------------------------- */
/*                            TOP‑HALF HANDLER                                */
/* -------------------------------------------------------------------------- */
/* Executes in hard‑IRQ context with interrupts disabled on the local CPU.
 * MUST be extremely fast: acknowledge the interrupt, queue the thread,
 * return IRQ_WAKE_THREAD so the kernel schedules the bottom half.            */
static irqreturn_t kb_top_handler(int irq, void *dev)
{
    pr_info("kbd_threaded: [TOP HALF] IRQ %d received\n", irq);
    return IRQ_WAKE_THREAD;          /* Ask the kernel to run kb_thread_handler()       */
}

/* -------------------------------------------------------------------------- */
/*                        BOTTOM‑HALF (threaded) HANDLER                       */
/* -------------------------------------------------------------------------- */
/* Runs in process context (a dedicated kthread), so it *may sleep*.
 * Do any heavy or blocking work here.                                         */
static irqreturn_t kb_thread_handler(int irq, void *dev)
{
    pr_info("kbd_threaded: [BOTTOM HALF] Handling key press…\n");

    /* Simulate time‑consuming processing (e.g., scan‑code decoding).         */
    msleep(500);

    pr_info("kbd_threaded: [BOTTOM HALF] Done processing\n");
    return IRQ_HANDLED;               /* Tell the core we’re finished.                  */
}

/* -------------------------------------------------------------------------- */
/*                         MODULE INITIALISATION                              */
/* -------------------------------------------------------------------------- */
static int __init kdb_irq_init(void)
{
    int ret;

    pr_info("kbd_threaded: Loading module\n");

    /* Request the interrupt line.
     *  - KBD_IRQ            : Interrupt number (1)
     *  - kb_top_handler     : Hard‑IRQ (top‑half) callback
     *  - kb_thread_handler  : Threaded (bottom‑half) callback
     *  - IRQF_SHARED        : Share the line with the normal AT keyboard driver
     *  - "kbd_threaded_irq" : Name visible in /proc/interrupts
     *  - &dev_id            : Our unique cookie                                       */
    ret = request_threaded_irq(KBD_IRQ,
                               kb_top_handler,
                               kb_thread_handler,
                               IRQF_SHARED,
                               "kbd_threaded_irq",
                               &dev_id);

    if (ret) {
        pr_err("kbd_threaded: Failed to request IRQ %d (err=%d)\n", KBD_IRQ, ret);
        return ret;                    /* Abort load if we can’t get the line           */
    }

    pr_info("kbd_threaded: IRQ %d registered successfully\n", KBD_IRQ);
    return 0;
}

/* -------------------------------------------------------------------------- */
/*                           MODULE CLEAN‑UP                                  */
/* -------------------------------------------------------------------------- */
static void __exit kdb_irq_exit(void)
{
    pr_info("kbd_threaded: Unloading module\n");

    /* Always free the IRQ you requested. Must pass the same *dev_id ptr.     */
    free_irq(KBD_IRQ, &dev_id);

    pr_info("kbd_threaded: IRQ %d freed\n", KBD_IRQ);
}

module_init(kdb_irq_init);
module_exit(kdb_irq_exit);

