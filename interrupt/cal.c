#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

static int a = 10, b = 2;
static int cookie; 

static int calculator(unsigned char scancode)
{
    switch (scancode) {
    case 0x1E: return a + b;  
    case 0x30: return a - b;  
    case 0x2E: return a * b; 
    case 0x20: return b ? a / b : 0;
    default:   return 0;
    }
}

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char sc = inb(KBD_DATA_PORT);
    if (sc == 0xE0 || sc & 0x80)
        return IRQ_HANDLED;

    printk(KERN_INFO "Calc result: %d (scancode 0x%02X)\n",
           calculator(sc), sc);
    return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
    int ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler,
                          IRQF_SHARED, "kbd_calc", &cookie);
    if (ret) {
        pr_err("kbd_calc: cannot register IRQ %d\n", KEYBOARD_IRQ);
        return ret;
    }
    pr_info("kbd_calc: loaded — press a/b/c/d\n");
    return 0;
}

static void __exit keyboard_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ, &cookie);
    pr_info("kbd_calc: unloaded\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Keyboard IRQ calculator");

