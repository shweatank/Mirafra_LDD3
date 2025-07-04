#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/string.h>

#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60

typedef int (*op_func)(int, int);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int divide(int a, int b) { return (b != 0) ? a / b : 0; }

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    if (scancode & 0x80)
        return IRQ_HANDLED; 
        

    
    printk(KERN_INFO "Scancode: 0x%02X\n", scancode);

    int a = 10, b = 5;  

    switch (scancode)
    {
        case 0x1E: 
            printk(KERN_INFO "ADD: %d + %d = %d\n", a, b, add(a, b));
            break;
        case 0x1F: 
            printk(KERN_INFO "SUB: %d - %d = %d\n", a, b, sub(a, b));
            break;
        case 0x32: 
            printk(KERN_INFO "MUL: %d * %d = %d\n", a, b, mul(a, b));
            break;
        case 0x20: 
            printk(KERN_INFO "DIV: %d / %d = %d\n", a, b, divide(a, b));
            break;
        default:
            break;
    }

    return IRQ_HANDLED;
}

static int __init calc_init(void) 
{
    return request_irq(KBD_IRQ, keyboard_irq_handler, IRQF_SHARED, "keyboard_calc", (void *)(keyboard_irq_handler));
}

static void __exit calc_exit(void) 
{
    free_irq(KBD_IRQ, (void *)(keyboard_irq_handler));
    printk(KERN_INFO "Unloaded keyboard calculator IRQ handler\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snigdha");
MODULE_DESCRIPTION("Desktop IRQ Keyboard Calculator");

