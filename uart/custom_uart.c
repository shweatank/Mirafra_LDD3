#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/io.h>

#define DEVICE_NAME "my_uart"
#define UART_BASE  0xFE201000  // PL011 UART base for BCM2711
#define UART_DR    0x00
#define UART_FR    0x18
#define UART_IBRD  0x24
#define UART_FBRD  0x28
#define UART_LCRH  0x2C
#define UART_CR    0x30
#define UART_IMSC  0x38
#define UART_ICR   0x44

static void __iomem *uart_base;

static int my_uart_open(struct inode *inode, struct file *file)
{
    // Disable UART before configuration
    writel(0x0, uart_base + UART_CR);

    // Clear pending interrupts
    writel(0x7FF, uart_base + UART_ICR);

    // Set integer & fractional part of baud rate
    // For 115200 baud rate with 48 MHz UART clock:
    // Integer part = 26, Fractional = 3 (approximated)
    writel(26, uart_base + UART_IBRD);
    writel(3, uart_base + UART_FBRD);

    // Set line control: 8 bits, no parity, 1 stop bit, FIFO enabled
    writel((1 << 4) | (3 << 5), uart_base + UART_LCRH);

    // Enable UART, TX, RX
    writel((1 << 0) | (1 << 8) | (1 << 9), uart_base + UART_CR);

    printk(KERN_INFO "UART Initialized: 115200-8N1\n");
    return 0;
}

static ssize_t my_uart_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    size_t i;
    char kbuf[256];
    if (len > sizeof(kbuf)) len = sizeof(kbuf);

    for (i = 0; i < len; i++) {
        while (readl(uart_base + UART_FR) & (1 << 4));  // Wait until RXFE == 0
        kbuf[i] = readl(uart_base + UART_DR) & 0xFF;
    }

    if (copy_to_user(buf, kbuf, len))
        return -EFAULT;

    return len;
}

static ssize_t my_uart_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    size_t i;
    char kbuf[256];
    if (len > sizeof(kbuf)) len = sizeof(kbuf);

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    for (i = 0; i < len; i++) {
        while (readl(uart_base + UART_FR) & (1 << 5));  // Wait until TXFF == 0
        writel(kbuf[i], uart_base + UART_DR);
    }

    return len;
}

static int my_uart_release(struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_uart_open,
    .read = my_uart_read,
    .write = my_uart_write,
    .release = my_uart_release,
};

static int __init my_uart_init(void)
{
    int ret;
    ret = register_chrdev(240, DEVICE_NAME, &fops);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to register char device\n");
        return ret;
    }

    uart_base = ioremap(UART_BASE, 0x100);
    if (!uart_base) {
        unregister_chrdev(240, DEVICE_NAME);
        printk(KERN_ALERT "Failed to ioremap UART\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "Custom UART driver loaded\n");
    return 0;
}

static void __exit my_uart_exit(void)
{
    iounmap(uart_base);
    unregister_chrdev(240, DEVICE_NAME);
    printk(KERN_INFO "Custom UART driver unloaded\n");
}

module_init(my_uart_init);
module_exit(my_uart_exit);
MODULE_LICENSE("GPL");

