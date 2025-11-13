#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/cdev.h>

#define DEVICE_NAME "my_uart"
#define UART0_BASE  0xFE201000  // PL011 UART base on BCM2711

#define UART_DR     0x00
#define UART_FR     0x18
#define UART_CR     0x30

#define FR_TXFF     (1 << 5)  // Transmit FIFO Full
#define FR_RXFE     (1 << 4)  // Receive FIFO Empty

static void __iomem *uart_base;
static int major;

static int my_uart_open(struct inode *inode, struct file *file) {
    pr_info("UART device opened\n");
    return 0;
}

static int my_uart_release(struct inode *inode, struct file *file) {
    pr_info("UART device closed\n");
    return 0;
}

static ssize_t my_uart_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    char kbuf[256];
    size_t i;

    len = min(len, sizeof(kbuf));
    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    for (i = 0; i < len; i++) {
        // Wait until TX FIFO is not full
        while (readl(uart_base + UART_FR) & FR_TXFF);
        writel(kbuf[i], uart_base + UART_DR);
    }

    return len;
}

/*static ssize_t my_uart_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    char kbuf[256];
    size_t i = 0;

    while (i < len) {
        // Wait until RX FIFO has data
        while (readl(uart_base + UART_FR) & FR_RXFE);
        kbuf[i++] = readl(uart_base + UART_DR) & 0xFF;
    }

    if (copy_to_user(buf, kbuf, i))
        return -EFAULT;

    return i;
}*/

#define UART_BUFFER_SIZE 256

static ssize_t my_uart_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    char kbuf[UART_BUFFER_SIZE];
    size_t i = 0;

    if (len > UART_BUFFER_SIZE)
        len = UART_BUFFER_SIZE;

    // Check if data is available
    while (!(readl(uart_base + UART_FR) & FR_RXFE) && i < len) {
        kbuf[i++] = readl(uart_base + UART_DR) & 0xFF;
    }

    // If no data was available
    if (i == 0)
        return 0;  // or return -EAGAIN for non-blocking read

    if (copy_to_user(buf, kbuf, i))
        return -EFAULT;

    return i;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_uart_open,
    .release = my_uart_release,
    .read = my_uart_read,
    .write = my_uart_write,
};

static int __init my_uart_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register char device\n");
        return major;
    }

    uart_base = ioremap(UART0_BASE, 0x1000);
    if (!uart_base) {
        unregister_chrdev(major, DEVICE_NAME);
        pr_err("Failed to map UART registers\n");
        return -ENOMEM;
    }

    pr_info("PL011 UART driver loaded. Major: %d\n", major);
    return 0;
}

static void __exit my_uart_exit(void) {
    if (uart_base)
        iounmap(uart_base);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("PL011 UART driver unloaded\n");
}

module_init(my_uart_init);
module_exit(my_uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sanaboina Pavan");
MODULE_DESCRIPTION("Polling-based Custom UART Driver for BCM2711");

