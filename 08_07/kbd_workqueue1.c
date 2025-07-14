#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/kernel.h>
#include <asm/io.h>

#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Keyboard IRQ + Workqueue Example");

static struct workqueue_struct *my_wq;

static void work_fn(struct work_struct *work);
static DECLARE_WORK(work, work_fn);

static void work_fn(struct work_struct *work)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    if (!(scancode & 0x80))  
        printk(KERN_INFO "Key Pressed: Scancode = 0x%x\n", scancode);
}


static irqreturn_t irq_handler(int irq, void *dev_id)
{
    queue_work(my_wq, &work);
    return IRQ_HANDLED;
}

static int __init my_init(void)
{
    int ret;

    my_wq = alloc_workqueue("kbd_wq", WQ_UNBOUND, 0);
    if (!my_wq)
        return -ENOMEM;

    ret = request_irq(KBD_IRQ, irq_handler, IRQF_SHARED, "kbd_irq_handler", (void *)(irq_handler));
    if (ret) {
        pr_err("Failed to register IRQ\n");
        destroy_workqueue(my_wq);
        return ret;
    }

    printk(KERN_INFO "Keyboard IRQ Workqueue Module Loaded\n");
    return 0;
}

static void __exit my_exit(void)
{
    free_irq(KBD_IRQ, (void *)(irq_handler));
    destroy_workqueue(my_wq);
    printk(KERN_INFO "Keyboard IRQ Workqueue Module Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);
