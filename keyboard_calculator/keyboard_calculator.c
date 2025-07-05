#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

static int operand1 = 0;
static int operand2 = 0;
static char op = 0;
static int result = 0;
static int input_state = 0;// 0: input1, 1: op, 2: input2, 3: compute
static int temp_val = 0;
static unsigned char latest_scancode = 0;
char scancode_to_ascii[128] = {
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x2E] = 'c', [0x2D] = 'x',
    [0x2A] = 0, [0x36] = 0,
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0',
    [0x1C] = '\n'
};

void tasklet_fn(struct tasklet_struct *t);
DECLARE_TASKLET(my_tasklet, tasklet_fn);
irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    latest_scancode = inb(KBD_DATA_PORT);
    if (!(latest_scancode & 0x80))
        tasklet_schedule(&my_tasklet);
    return IRQ_HANDLED;
}
void tasklet_fn(struct tasklet_struct *t)
{
    char key = 0;
    if (latest_scancode < sizeof(scancode_to_ascii))
        key = scancode_to_ascii[latest_scancode];
    if (key >= '0' && key <= '9') {
        temp_val = temp_val * 10 + (key - '0');
    } else if (key == '\n') {
        if (input_state == 0) {
            operand1 = temp_val;
            temp_val = 0;
            input_state = 1;
            printk(KERN_INFO "Operand1 captured: %d\n", operand1);
        } else if (input_state == 2) {
            operand2 = temp_val;
            temp_val = 0;
            input_state = 3;
            printk(KERN_INFO "Operand2 captured: %d\n", operand2);
        } else if (input_state == 3) {
            switch (op) {
                case 'a': result = operand1 + operand2; break;
                case 's': result = operand1 - operand2; break;
                case 'd': result = operand2 ? operand1 / operand2 : 0; break;
                case 'm': result = operand1 * operand2; break;
                default: result = 0; break;
            }
            printk(KERN_INFO "operand1 = %d, operand2 = %d, op = %c\n", operand1, operand2, op);
            printk(KERN_INFO "Result: %d\n", result);
            operand1 = operand2 = result = 0;
            input_state = 0;
            temp_val = 0;
            op = 0;
        }
    } else if (key == 'a' || key == 's' || key == 'd' || key == 'm') {
        if (input_state == 1) {
            op = key;
            input_state = 2;
            printk(KERN_INFO "Operator captured: %c\n", op);
        }
    }
}
static int __init keyboard_irq_init(void)
{
    int ret;
    printk(KERN_INFO "Loading keyboard IRQ handler with tasklet...\n");

    ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED,
                      "keyboard_irq_handler", (void *)(keyboard_irq_handler));
    if (ret) {
        printk(KERN_ERR "Cannot register IRQ %d\n", KEYBOARD_IRQ);
        return ret;
    }
    printk(KERN_INFO "Keyboard IRQ handler registered successfully.\n");
    return 0;
}
static void __exit keyboard_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    tasklet_kill(&my_tasklet);
    printk(KERN_INFO "Keyboard IRQ handler removed.\n");
}
module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("Keyboard driver with tasklet - arithmetic");

