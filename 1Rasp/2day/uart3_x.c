#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <linux/io.h>

#define UART_PORT 0x3F8 // COM1 I/O Port
#define BUF_SIZE 128

static char input_buf[BUF_SIZE];
static int input_pos = 0;

// ------------------- Tasklet Function -------------------

static void tasklet_uart_send_func(struct tasklet_struct *unused);

// Declare Tasklet (modern kernel uses only 2 args)
DECLARE_TASKLET(tasklet_uart_send, tasklet_uart_send_func);

// ------------------- IRQ Handler -------------------

static irqreturn_t kb_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(0x60);

    // Enter key (0x1C for key press)
    if (scancode == 0x1C) {
        input_buf[input_pos] = '\0';  // Null terminate
        tasklet_schedule(&tasklet_uart_send);
        input_pos = 0;
    }
    else {
        if (input_pos < BUF_SIZE - 1) {
            input_buf[input_pos++] = scancode;
        }
    }

    return IRQ_HANDLED;
}

// ------------------- Tasklet Body -------------------

static void tasklet_uart_send_func(struct tasklet_struct *unused)
{
    int i;

    for (i = 0; input_buf[i] != '\0'; i++) {
        // Wait for UART to be ready
        while (!(inb(UART_PORT + 5) & 0x20));
        outb(input_buf[i], UART_PORT);
    }

    // Send newline at end
    while (!(inb(UART_PORT + 5) & 0x20));
    outb('\n', UART_PORT);

    pr_info("x86: Sent string to RPi: %s\n", input_buf);
}

// ------------------- Module Init -------------------

static int __init kb_uart_init(void)
{
    if (!request_region(UART_PORT, 8, "uart_port")) {
        pr_err("UART port already in use\n");
        return -EBUSY;
    }

    // UART config (115200 baud, 8N1)
    outb(0x80, UART_PORT + 3); // Enable DLAB
    outb(0x01, UART_PORT + 0); // Divisor low byte
    outb(0x00, UART_PORT + 1); // Divisor high byte
    outb(0x03, UART_PORT + 3); // 8 bits, no parity, 1 stop
    outb(0xC7, UART_PORT + 2); // Enable FIFO, clear them
    outb(0x0B, UART_PORT + 4); // IRQs enabled, RTS/DSR set

    if (request_irq(1, kb_irq_handler, IRQF_SHARED, "keyboard_uart", (void *)kb_irq_handler)) {
        release_region(UART_PORT, 8);
        pr_err("Failed to register IRQ 1\n");
        return -EIO;
    }

    pr_info("x86: Keyboard UART module loaded\n");
    return 0;
}

// ------------------- Module Exit -------------------

static void __exit kb_uart_exit(void)
{
    tasklet_kill(&tasklet_uart_send);
    free_irq(1, (void *)kb_irq_handler);
    release_region(UART_PORT, 8);
    pr_info("x86: Keyboard UART module unloaded\n");
}

module_init(kb_uart_init);
module_exit(kb_uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("x86 Keyboard to UART tasklet using IRQ and tasklet");

