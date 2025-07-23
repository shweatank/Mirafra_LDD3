#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define UART0_BASE 0xFE201000  // PL011 base for RPi 4
#define UART_FR    0x18        // Flag Register
#define UART_DR    0x00        // Data Register
#define UART_CR    0x30        // Control Register
#define UART_IBRD  0x24        // Integer Baud Rate
#define UART_FBRD  0x28        // Fractional Baud Rate
#define UART_LCRH  0x2C        // Line Control
#define UART_IMSC  0x38        // Interrupt Mask

#define DEVICE_NAME "uart_hello"
#define CLASS_NAME  "uart_class"

static void __iomem *uart_base;
static int major;
static struct class *uart_class = NULL;
static struct device *uart_device = NULL;

static void uart_send_char(char c) {
    while (readl(uart_base + UART_FR) & (1 << 5));
    writel(c, uart_base + UART_DR);
}

static void uart_send_string(const char *s) {
    while (*s) {
        uart_send_char(*s++);
    }
}

static ssize_t uart_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    char kbuf[256];
    size_t to_copy = min(len, sizeof(kbuf) - 1);

    if (copy_from_user(kbuf, buf, to_copy))
        return -EFAULT;

    kbuf[to_copy] = '\0';
    uart_send_string(kbuf);
    return to_copy;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = uart_write,
};

static int __init uart_init(void) {
    pr_info("UART Hello Driver Init\n");

    uart_base = ioremap(UART0_BASE, 0x100);
    if (!uart_base) {
        pr_err("UART ioremap failed\n");
        return -ENOMEM;
    }

    writel(0x0, uart_base + UART_CR);
    writel(26, uart_base + UART_IBRD);
    writel(3, uart_base + UART_FBRD);
    writel((3 << 5) | (1 << 4), uart_base + UART_LCRH);
    writel(0x0, uart_base + UART_IMSC);
    writel((1 << 0) | (1 << 8) | (1 << 9), uart_base + UART_CR);

    uart_send_string("Hello\n");

    // Register char device
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register char device\n");
        iounmap(uart_base);
        return major;
    }

    uart_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(uart_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        iounmap(uart_base);
        return PTR_ERR(uart_class);
    }

    uart_device = device_create(uart_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(uart_device)) {
        class_destroy(uart_class);
        unregister_chrdev(major, DEVICE_NAME);
        iounmap(uart_base);
        return PTR_ERR(uart_device);
    }

    pr_info("UART device created at /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit uart_exit(void) {
    device_destroy(uart_class, MKDEV(major, 0));
    class_destroy(uart_class);
    unregister_chrdev(major, DEVICE_NAME);
    writel(0x0, uart_base + UART_CR);
    iounmap(uart_base);
    pr_info("UART Hello Driver Exit\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba + Sumanth");
MODULE_DESCRIPTION("Minimal UART driver with /dev/uart_hello support");

