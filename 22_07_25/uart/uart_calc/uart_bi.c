#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define UART0_BASE 0xFE201000  // PL011 base (Raspberry Pi 4)
#define UART_DR    0x00
#define UART_FR    0x18
#define UART_IBRD  0x24
#define UART_FBRD  0x28
#define UART_LCRH  0x2C
#define UART_CR    0x30
#define UART_IMSC  0x38

static void __iomem *uart_base;
static struct task_struct *rx_thread;
#define DEVICE_NAME "uart_bidir"

static int major;
static struct class *uart_class;
static struct device *uart_device;
#define BUF_SIZE 128
static char last_rx_char = 0;

static void uart_send_char(char c) {
    while (readl(uart_base + UART_FR) & (1 << 5));
    writel(c, uart_base + UART_DR);
}

static void uart_send_string(const char *s) {
    while (*s) {
        uart_send_char(*s++);
    }
}

static char uart_recv_char(void) {
    return readl(uart_base + UART_DR) & 0xFF;
}

static int uart_rx_thread(void *data) {
    char c;
    while (!kthread_should_stop()) {
        if (!(readl(uart_base + UART_FR) & (1 << 4))) {
            c = uart_recv_char();
            last_rx_char = c;
            uart_send_string("Received: ");
            uart_send_char(c);
            uart_send_char('\n');
        }
        msleep(100);
    }
    return 0;
}

// ------------ Character device operations ------------------

static ssize_t uart_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    char tmp = last_rx_char;
    if (copy_to_user(buf, &tmp, 1))
        return -EFAULT;
    return 1;
}

static ssize_t uart_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    char kbuf[BUF_SIZE] = {0};
    if (count > BUF_SIZE - 1)
        count = BUF_SIZE - 1;

    if (copy_from_user(kbuf, buf, count))
        return -EFAULT;

    uart_send_string("User: ");
    uart_send_string(kbuf);
    uart_send_char('\n');
    return count;
}

static int uart_open(struct inode *inode, struct file *file) {
    return 0;
}

static int uart_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = uart_open,
    .read = uart_read,
    .write = uart_write,
    .release = uart_release,
};

// ------------------------------------------------------------

static int __init uart_init(void) {
    pr_info("UART Bidirectional Driver Init\n");

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

    // Register char device
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register char device\n");
        return major;
    }

    uart_class = class_create("uart_class");
    if (IS_ERR(uart_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(uart_class);
    }

    uart_device = device_create(uart_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(uart_device)) {
        class_destroy(uart_class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(uart_device);
    }

    uart_send_string("UART initialized via char device\n");

    rx_thread = kthread_run(uart_rx_thread, NULL, "uart_rx_thread");
    if (IS_ERR(rx_thread)) {
        device_destroy(uart_class, MKDEV(major, 0));
        class_destroy(uart_class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(rx_thread);
    }

    return 0;
}

static void __exit uart_exit(void) {
    if (rx_thread)
        kthread_stop(rx_thread);

    writel(0x0, uart_base + UART_CR);
    iounmap(uart_base);

    device_destroy(uart_class, MKDEV(major, 0));
    class_destroy(uart_class);
    unregister_chrdev(major, DEVICE_NAME);

    pr_info("UART Bidirectional Driver Exit\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("Bi-directional UART driver with char device and dynamic major");
