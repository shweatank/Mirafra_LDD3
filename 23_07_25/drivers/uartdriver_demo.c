// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/delay.h>

#define UART0_BASE  0xFE201000
#define UART2_BASE  0xFE201400

#define UART_DR     0x00
#define UART_FR     0x18
#define UART_CR     0x30

#define FR_RXFE     (1 << 4)
#define FR_TXFF     (1 << 5)

#define DEVICE_NAME "dual_uart"
#define TIMEOUT_MS  5000  // Timeout for read in milliseconds

static void __iomem *uart0_base;
static void __iomem *uart2_base;
static int major;

static int uart_open(struct inode *inode, struct file *file) {
    writel(0x0, uart0_base + UART_CR);
    writel(0x0, uart2_base + UART_CR);

    writel((1 << 0) | (1 << 8) | (1 << 9), uart0_base + UART_CR);
    writel((1 << 0) | (1 << 8) | (1 << 9), uart2_base + UART_CR);

    return 0;
}

static ssize_t uart_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    size_t i;
    char ch;

    for (i = 0; i < len; i++) {
        if (copy_from_user(&ch, buf + i, 1))
            return -EFAULT;

        while (readl(uart0_base + UART_FR) & FR_TXFF);
        writel(ch, uart0_base + UART_DR);
    }

    return len;
}

static ssize_t uart_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    char ch;
    size_t count = 0;
    unsigned long timeout;

    while (count < len) {
        timeout = jiffies + msecs_to_jiffies(TIMEOUT_MS);

        // Wait until data is available or timeout
        while ((readl(uart2_base + UART_FR) & FR_RXFE)) {
            if (time_after(jiffies, timeout)) {
                pr_warn("UART2 read timeout\n");
                return count ? count : -EAGAIN;
            }
            msleep(10);
        }

        ch = readl(uart2_base + UART_DR) & 0xFF;
        ch++;  // Increment character

        // Echo back to UART0
        while (readl(uart0_base + UART_FR) & FR_TXFF);
        writel(ch, uart0_base + UART_DR);

        if (copy_to_user(buf + count, &ch, 1))
            return -EFAULT;

        count++;
    }

    return count;
}

static struct file_operations uart_fops = {
    .owner = THIS_MODULE,
    .open = uart_open,
    .read = uart_read,
    .write = uart_write,
};

static int __init uart_init(void) {
    int ret;

    major = register_chrdev(0, DEVICE_NAME, &uart_fops);
    if (major < 0) {
        pr_err("Failed to register character device\n");
        return major;
    }

    uart0_base = ioremap(UART0_BASE, 0x100);
    uart2_base = ioremap(UART2_BASE, 0x100);

    if (!uart0_base || !uart2_base) {
        pr_err("Failed to ioremap UARTs\n");
        if (uart0_base) iounmap(uart0_base);
        if (uart2_base) iounmap(uart2_base);
        unregister_chrdev(major, DEVICE_NAME);
        return -ENOMEM;
    }

    pr_info("Dual UART driver loaded. Major: %d\n", major);
    return 0;
}

static void __exit uart_exit(void) {
    if (uart0_base) {
        writel(0x0, uart0_base + UART_CR);
        iounmap(uart0_base);
    }

    if (uart2_base) {
        writel(0x0, uart2_base + UART_CR);
        iounmap(uart2_base);
    }

    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Dual UART driver unloaded\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanth G, updated by ChatGPT");
MODULE_DESCRIPTION("Dual UART0/UART2 kernel module on RPi4 with loopback logic");

