#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/io.h>
#include <linux/input.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Fixed Calculator using IRQ, Kthread, Workqueue, Waitqueue");

#define BUFFER_LEN 100
static char calc_buffer[BUFFER_LEN];
static int buf_index = 0;
static bool shift_pressed = false;

static int irq = 1; // keyboard IRQ
static int data_ready = 0;

static wait_queue_head_t calc_wq;
static struct task_struct *calc_kthread;
static struct workqueue_struct *calc_wq_struct;
static struct work_struct calc_work;
static char keymap_normal[128] = {
    [2] = '1', [3] = '2', [4] = '3', [5] = '4',
    [6] = '5', [7] = '6', [8] = '7', [9] = '8',
    [10] = '9', [11] = '0', [12] = '-', [13] = '=',
    [14] = '\b', [28] = '\n', [53] = '/', [55] = '*', [74] = '-', [78] = '+',
    [83] = '='
};

static char keymap_shift[128] = {
    [2] = '!', [3] = '@', [4] = '#', [5] = '$',
    [6] = '%', [7] = '^', [8] = '&', [9] = '*',
    [10] = '(', [11] = ')', [12] = '_', [13] = '+',
    [14] = '\b', [28] = '\n', [53] = '/', [55] = '*', [74] = '-', [78] = '+',
    [83] = '='
};

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id) {
    unsigned char scancode = inb(0x60);

    // Left Shift press = 0x2A, release = 0xAA
    // Right Shift press = 0x36, release = 0xB6
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = true;
        return IRQ_HANDLED;
    } else if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = false;
        return IRQ_HANDLED;
    }

    // Ignore key releases (high bit = 1)
    if (scancode & 0x80)
        return IRQ_HANDLED;

    char ch = shift_pressed ? keymap_shift[scancode] : keymap_normal[scancode];

    if (ch) {
        if (ch == '=' || ch == '\n') {
            calc_buffer[buf_index] = '\0'; // null terminate
            data_ready = 1;
            wake_up_interruptible(&calc_wq);
        } else if (ch == '\b') {
            if (buf_index > 0)
                buf_index--;
        } else {
            if (buf_index < BUFFER_LEN - 1) {
                calc_buffer[buf_index++] = ch;
                calc_buffer[buf_index] = '\0'; // keep buffer clean
            }
        }
    }

    return IRQ_HANDLED;
}

// ---- Workqueue Function ----
static void calc_do_work(struct work_struct *work) {
    int num1 = 0, num2 = 0, result = 0;
    char op = 0;
    int i = 0;

    printk(KERN_INFO "Raw expression: [%s]\n", calc_buffer);

    // Parse number1
    while (calc_buffer[i] >= '0' && calc_buffer[i] <= '9')
        num1 = num1 * 10 + (calc_buffer[i++] - '0');

    // Get operator
    op = calc_buffer[i++];

    // Parse number2
    while (calc_buffer[i] >= '0' && calc_buffer[i] <= '9')
        num2 = num2 * 10 + (calc_buffer[i++] - '0');

    // Check if operator is valid
    switch (op) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/': result = (num2 != 0) ? num1 / num2 : 0; break;
        default:
            printk(KERN_ERR "Invalid operator: [%c]\n", op);
            goto cleanup;
    }

    printk(KERN_INFO "Calculation: %d %c %d = %d\n", num1, op, num2, result);

cleanup:
    // Reset buffer after processing
    buf_index = 0;
    memset(calc_buffer, 0, BUFFER_LEN);
}

// ---- Kthread Function ----
static int calc_thread_fn(void *data) {
    while (!kthread_should_stop()) {
        wait_event_interruptible(calc_wq, data_ready != 0);
        if (data_ready) {
            data_ready = 0;
            queue_work(calc_wq_struct, &calc_work);
        }
    }
    return 0;
}

// ---- Module Init ----
static int __init calc_init(void) {
    int ret;

    init_waitqueue_head(&calc_wq);
    buf_index = 0;

    ret = request_irq(irq, keyboard_irq_handler, IRQF_SHARED, "calc_keyboard", (void *)(keyboard_irq_handler));
    if (ret) {
        printk(KERN_ERR "Failed to register IRQ %d\n", irq);
        return ret;
    }

    calc_wq_struct = create_singlethread_workqueue("calc_wq");
    if (!calc_wq_struct) {
        free_irq(irq, (void *)(keyboard_irq_handler));
        return -ENOMEM;
    }

    INIT_WORK(&calc_work, calc_do_work);

    calc_kthread = kthread_run(calc_thread_fn, NULL, "calc_kthread");
    if (IS_ERR(calc_kthread)) {
        destroy_workqueue(calc_wq_struct);
        free_irq(irq, (void *)(keyboard_irq_handler));
        return PTR_ERR(calc_kthread);
    }

    printk(KERN_INFO "Calculator module loaded. Type expressions like 12+8= or 9*3=.\n");
    return 0;
}

// ---- Module Exit ----
static void __exit calc_exit(void) {
    kthread_stop(calc_kthread);
    flush_workqueue(calc_wq_struct);
    destroy_workqueue(calc_wq_struct);
    free_irq(irq, (void *)(keyboard_irq_handler));
    printk(KERN_INFO "Calculator module unloaded.\n");
}

module_init(calc_init);
module_exit(calc_exit);

