#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/string.h>

#define KEYBOARD_IRQ     1
#define KBD_DATA_PORT    0x60

#define MAX_INPUT_LEN 32
static char input_buffer[MAX_INPUT_LEN];
static int buf_index = 0;

static char scancode_ascii[128] =
{
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0',
    [0x0C] = '-', [0x4E] = '+', [0x37] = '*', [0x35] = '/',
    [0x1C] = '=', // Enter is treated as '='
};

irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);
    char ch;

    // Ignore key releases (MSB set)
    if (scancode & 0x80)
        return IRQ_HANDLED;

    ch = scancode_ascii[scancode];

    if (ch != 0) {
        if (ch == '=') {
            int a = 0, b = 0, result = 0;
            char op = 0;

            // Ensure null-terminated string
            input_buffer[buf_index] = '\0';

            // Parse basic expression like "12+34"
            sscanf(input_buffer, "%d%c%d", &a, &op, &b);

            switch (op) 
	    {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': result = (b != 0) ? a / b : -1; break;
                default:
                    printk(KERN_INFO "IRQ Calc: Invalid operator '%c'\n", op);
                    return IRQ_HANDLED;
            }

            printk(KERN_INFO "IRQ Calc: %d %c %d = %d\n", a, op, b, result);

            // Clear buffer
            buf_index = 0;
            memset(input_buffer, 0, MAX_INPUT_LEN);
        } else {
            if (buf_index < MAX_INPUT_LEN - 1)
                input_buffer[buf_index++] = ch;
        }
    }

    return IRQ_HANDLED;
}

static int __init irq_calc_init(void)
{
    int ret;

    printk(KERN_INFO "IRQ Calc: Loading calculator with inline logic in IRQ handler...\n");

    ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED,
                      "keyboard_calc_irq", (void *)keyboard_irq_handler);

    if (ret) {
        printk(KERN_ERR "IRQ Calc: Failed to register IRQ %d\n", KEYBOARD_IRQ);
        return ret;
    }

    return 0;
}

static void __exit irq_calc_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)keyboard_irq_handler);
    printk(KERN_INFO "IRQ Calc: Unloaded keyboard calculator module\n");
}

module_init(irq_calc_init);
module_exit(irq_calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Calculator using inline logic in IRQ handler");

