#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mamta Modified by ChatGPT");
MODULE_DESCRIPTION("Calculator using Keyboard IRQ Handler");

static bool key_pressed[256] = { false };

static int operand1 = 0, operand2 = 0;
static char operator = 0;
static bool entering_second = false;

static int scancode_to_digit(unsigned char code)
{
    if (code >= 0x02 && code <= 0x0B) {
        if (code == 0x0B) return 0;  // Scancode for '0'
        return code - 0x01;
    }
    return -1;
}

irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);
    unsigned char clean_code = scancode & 0x7F;
    int digit;
    if (!(scancode & 0x80)) {
        if (!key_pressed[clean_code]) {
            key_pressed[clean_code] = true;

            digit = scancode_to_digit(clean_code);

            if (digit >= 0) {
                if (!entering_second) {
                    operand1 = operand1 * 10 + digit;
                } else {
                    operand2 = operand2 * 10 + digit;
                }
            } else {
                switch (clean_code) {
                case 0x4E:  // '+'
                    operator = '+';
                    entering_second = true;
                    break;
                case 0x4A:  // '-'
                    operator = '-';
                    entering_second = true;
                    break;
                case 0x37:  // '*'
                    operator = '*';
                    entering_second = true;
                    break;
                case 0x35:  // '/'
                    operator = '/';
                    entering_second = true;
                    break;
                case 0x2E:  // 'C' to clear
                    operand1 = operand2 = 0;
                    operator = 0;
                    entering_second = false;
                    printk(KERN_INFO "Calculator: Cleared\n");
                    break;
                case 0x1C:  // Enter key
                    if (operator) {
                        int result = 0;
                        bool error = false;
                        switch (operator) {
                            case '+': result = operand1 + operand2; break;
                            case '-': result = operand1 - operand2; break;
                            case '*': result = operand1 * operand2; break;
                            case '/':
                                if (operand2 == 0) {
                                    printk(KERN_ERR "Calculator Error: Division by zero\n");
                                    error = true;
                                } else {
                                    result = operand1 / operand2;
                                }
                                break;
                        }

                        if (!error) {
                            printk(KERN_INFO "Calculator Result: %d %c %d = %d\n",
                                operand1, operator, operand2, result);
                        }
                    } else {
                        printk(KERN_INFO "Calculator Error: Operator not set\n");
                    }

                    // Reset calculator after computation
                    operand1 = operand2 = 0;
                    operator = 0;
                    entering_second = false;
                    break;
                default:
                    // Unused key
                    break;
                }
            }
        }
    } else {
        key_pressed[clean_code] = false;  // Key release
    }

    return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
    int result;
    printk(KERN_INFO "Loading calculator keyboard IRQ handler...\n");

    result = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED,
                         "keyboard_irq_handler", (void *)(keyboard_irq_handler));

    if (result) {
        printk(KERN_ERR "keyboard_irq: Cannot register IRQ %d\n", KEYBOARD_IRQ);
        return result;
    }

    printk(KERN_INFO "keyboard_irq: IRQ handler registered successfully\n");
    return 0;
}

static void __exit keyboard_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    printk(KERN_INFO "keyboard_irq: IRQ handler removed\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

