#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

static char input_buffer[20];
static int index = 0;

char scancode_to_char(unsigned char sc)
{
    switch (sc) {
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';
        case 0x4E: return '+';
        case 0x4A: return '-';
        case 0x37: return '*';
        case 0x35: return '/';
        case 0x1C: return '='; // use Enter as '='
        default: return '\0';
    }
}

int perform_calculation(char *expr)
{
    int a = 4, b =2;
    char op = 0;
    sscanf(expr, "%d%c%d", &a, &op, &b);
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return (b != 0) ? a / b : 0;
        default: return 0;
    }
}

irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);
    char ch = scancode_to_char(scancode);

    if (ch != '\0') {
        if (ch == '=') {
            input_buffer[index] = '\0';
            int result = perform_calculation(input_buffer);
            printk(KERN_INFO "Calculation: %s = %d\n", input_buffer, result);
            index = 0;
        } else if (index < sizeof(input_buffer) - 1) {
            input_buffer[index++] = ch;
        }
    }

    return IRQ_HANDLED;
}

static int __init calculator_irq_init(void)
{
    int result;
    printk(KERN_INFO "Loading keyboard IRQ calculator module...\n");

    result = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED,
                         "keyboard_irq_calc", (void *)(keyboard_irq_handler));
    if (result) {
        printk(KERN_ERR "Failed to register IRQ handler.\n");
        return result;
    }
    printk(KERN_INFO "Keyboard IRQ calculator registered.\n");
    return 0;
}

static void __exit calculator_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    printk(KERN_INFO "Keyboard IRQ calculator removed.\n");
}

module_init(calculator_irq_init);
module_exit(calculator_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adinarayana");
MODULE_DESCRIPTION("Keyboard-based Calculator using IRQ");

