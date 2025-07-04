#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/workqueue.h>
#include <linux/string.h>

#define KBD_IRQ         1
#define KBD_DATA_PORT   0x60
#define BUF_SIZE        100

static char input_buffer[BUF_SIZE];
static int buf_index = 0;

// Work structure
static struct work_struct kb_work;

// Partial scancode-to-ASCII map (no shift support)
char scancode_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','+','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ', 0
};

// Expression evaluator
static void evaluate_expression(char *expr)
{
    int a = 0, b = 0;
    char op;

    if (sscanf(expr, "%d%c%d", &a, &op, &b) != 3) {
        pr_info("Invalid expression: %s\n", expr);
        return;
    }

    int result;
    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/': 
            if (b == 0) {
                pr_info("Divide by zero error\n");
                return;
            }
            result = a / b;
            break;
        default:
            pr_info("Unknown operator: %c\n", op);
            return;
    }

    pr_info("Result: %d %c %d = %d\n", a, op, b, result);
}

// Workqueue function
static void kb_work_handler(struct work_struct *work)
{
    input_buffer[buf_index] = '\0';
    pr_info("Received expression: %s\n", input_buffer);
    evaluate_expression(input_buffer);
    buf_index = 0;
}

// Interrupt handler
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    if (scancode < 128) {
        char key = scancode_map[scancode];

        if (key == '\n') {
            schedule_work(&kb_work);  // defer evaluation
        } else if (buf_index < BUF_SIZE - 1 && key != 0) {
            input_buffer[buf_index++] = key;
        }
    }

    return IRQ_HANDLED;
}

// Init function
static int __init kbd_calc_init(void)
{
    pr_info("Keyboard Calculator Driver Loaded\n");

    INIT_WORK(&kb_work, kb_work_handler);

    if (request_irq(KBD_IRQ, keyboard_irq_handler, IRQF_SHARED,
                    "kbd_calc_irq", (void *)(keyboard_irq_handler))) {
        pr_err("Failed to register IRQ %d\n", KBD_IRQ);
        return -EIO;
    }

    return 0;
}

// Exit function
static void __exit kbd_calc_exit(void)
{
    free_irq(KBD_IRQ, (void *)(keyboard_irq_handler));
    flush_scheduled_work(); // ensure pending work is completed
    pr_info("Keyboard Calculator Driver Unloaded\n");
}

module_init(kbd_calc_init);
module_exit(kbd_calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("Keyboard Calculator Driver using IRQ and Workqueue");

