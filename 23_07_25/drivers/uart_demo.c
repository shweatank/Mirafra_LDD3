#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define UART0_BASE  0xFE201000  // PL011 UART0
#define UART2_BASE  0xFE215000  // Mini UART (AUX)

#define UART_DR     0x00        // Data register
#define UART_FR     0x18        // Flag register
#define UART_CR     0x30        // Control register

#define FR_TXFF     (1 << 5)    // Transmit FIFO full
#define FR_RXFE     (1 << 4)    // Receive FIFO empty

static void __iomem *uart0_base;
static void __iomem *uart2_base;
static struct task_struct *worker_thread;

// Background thread to handle UART communication
static int uart_thread_fn(void *data)
{
    char ch;

    pr_info("UART thread started\n");

    while (!kthread_should_stop()) {

        // Wait for data from UART0
        if (!(readl(uart0_base + UART_FR) & FR_RXFE)) {
            ch = readl(uart0_base + UART_DR) & 0xFF;
            ch++;  // Increment the character

            // Wait for UART2 TX ready
            while (readl(uart2_base + UART_FR) & FR_TXFF);
            writel(ch, uart2_base + UART_DR);

            // Echo back to UART0
            while (readl(uart0_base + UART_FR) & FR_TXFF);
            writel(ch, uart0_base + UART_DR);

            pr_info("Received '%c', sent '%c'\n", ch - 1, ch);
        }

        msleep(10); // Prevent 100% CPU usage
    }

    return 0;
}

static int __init dual_uart_init(void)
{
    pr_info("Initializing Dual UART driver\n");

    // Map UART0 and UART2
    uart0_base = ioremap(UART0_BASE, 0x100);
    uart2_base = ioremap(UART2_BASE, 0x100);

    if (!uart0_base || !uart2_base) {
        pr_err("Failed to map UART addresses\n");
        return -ENOMEM;
    }

    // Enable UARTs by setting UART_CR (optional if already enabled by firmware)
    writel(0x301, uart0_base + UART_CR);  // Enable TX, RX, UART0
    writel(0x301, uart2_base + UART_CR);  // Enable TX, RX, UART2

    // Create and start the kernel thread
    worker_thread = kthread_run(uart_thread_fn, NULL, "uart_thread");
    if (IS_ERR(worker_thread)) {
        pr_err("Failed to create UART thread\n");
        return PTR_ERR(worker_thread);
    }

    return 0;
}

static void __exit dual_uart_exit(void)
{
    pr_info("Exiting Dual UART driver\n");

    if (worker_thread)
        kthread_stop(worker_thread);

    if (uart0_base)
        iounmap(uart0_base);

    if (uart2_base)
        iounmap(uart2_base);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanth");
MODULE_DESCRIPTION("Custom Dual UART Driver for Raspberry Pi");
MODULE_VERSION("1.0");

module_init(dual_uart_init);
module_exit(dual_uart_exit);

