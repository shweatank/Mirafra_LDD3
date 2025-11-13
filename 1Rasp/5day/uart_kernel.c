// uart_kernel.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define UART0_BASE 0xFE201000  // PL011 base for RPi 4
#define UART_FR    0x18        // Flag Register
#define UART_DR    0x00        // Data Register
#define UART_CR    0x30        // Control Register
#define UART_IBRD  0x24        // Integer Baud Rate
#define UART_FBRD  0x28        // Fractional Baud Rate
#define UART_LCRH  0x2C        // Line Control
#define UART_IMSC  0x38        // Interrupt Mask

static void __iomem *uart_base;
static struct task_struct *uart_thread;

// Send one character
static void uart_send_char(char c) {
    while (readl(uart_base + UART_FR) & (1 << 5));  // Wait until TX FIFO not full
    writel(c, uart_base + UART_DR);
}

// Send string
static void uart_send_string(const char *s) {
    while (*s) {
        uart_send_char(*s++);
    }
}

// Thread function to send "hi\n" every 5 seconds
static int uart_thread_fn(void *data) {
    while (!kthread_should_stop()) {
        uart_send_string("hi\n");
        ssleep(5);
    }
    return 0;
}

// Init function
static int __init uart_init(void) {
    pr_info("UART Hello Driver Init\n");

    uart_base = ioremap(UART0_BASE, 0x100);
    if (!uart_base) {
        pr_err("UART ioremap failed\n");
        return -ENOMEM;
    }

    // Disable UART
    writel(0x0, uart_base + UART_CR);

    // Baud rate = 48 MHz / (16 * 26.1875) = 115200
    writel(26, uart_base + UART_IBRD);
    writel(3, uart_base + UART_FBRD);

    // 8N1, enable FIFO
    writel((3 << 5) | (1 << 4), uart_base + UART_LCRH);

    // Disable UART interrupts
    writel(0x0, uart_base + UART_IMSC);

    // Enable UART, TX and RX
    writel((1 << 0) | (1 << 8) | (1 << 9), uart_base + UART_CR);

    // Start UART thread
    uart_thread = kthread_run(uart_thread_fn, NULL, "uart_hi_thread");
    if (IS_ERR(uart_thread)) {
        pr_err("Failed to create UART thread\n");
        iounmap(uart_base);
        return PTR_ERR(uart_thread);
    }

    return 0;
}

// Exit function
static void __exit uart_exit(void) {
    // Stop thread
    if (uart_thread)
        kthread_stop(uart_thread);

    // Disable UART
    writel(0x0, uart_base + UART_CR);

    // Unmap memory
    iounmap(uart_base);

    pr_info("UART Hello Driver Exit\n");
}

module_init(uart_init); 
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("UART driver that sends 'hi' over GPIO14 TX every 5 seconds");

