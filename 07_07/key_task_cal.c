#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/smp.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

static int a = 10, b = 2;
static int cookie;

static void my_tasklet_func(struct tasklet_struct *tasklet);

static struct tasklet_struct my_tasklet;

static void my_tasklet_func(struct tasklet_struct *tasklet) {
    int result = 0;
    pr_info("Tasklet: Executing in softirq context on CPU %u\n", smp_processor_id());

    switch ((u8)tasklet->data) {
        case 0x1E: result = a + b; break;
        case 0x30: result = a - b; break;
        case 0x2E: result = a * b; break;
        case 0x20: result = b ? a / b : 0; break;
        default:
            printk("invalid input\n");
            return;
    }
    printk("result is %d\n", result);
}

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id) {
    unsigned char sc = inb(KBD_DATA_PORT);

    if (!(sc & 0x80)) {  // reacting to key press (bit7 clear)
        my_tasklet.data = sc;
        tasklet_schedule(&my_tasklet);
    }

    return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void) {
    int ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler,
                          IRQF_SHARED, "kbd_calc", &cookie);
    if (ret) {
        pr_err("kbd_calc: cannot register IRQ %d\n", KEYBOARD_IRQ);
        return ret;
    }
    tasklet_setup(&my_tasklet, my_tasklet_func);
    pr_info("kbd_calc: loaded — press a/b/c/d\n");
    return 0;
}

static void __exit keyboard_irq_exit(void) {
    free_irq(KEYBOARD_IRQ, &cookie);
    tasklet_kill(&my_tasklet);
    pr_info("kbd_calc: unloaded\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Keyboard IRQ calculator");

