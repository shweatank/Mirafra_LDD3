#include <linux/module.h>       // For all kernel modules
#include <linux/kernel.h>       // For KERN_INFO, printk()
#include <linux/fs.h>           // For file_operations structure
#include <linux/uaccess.h>      // For copy_to_user and copy_from_user
#include <linux/init.h>         // For __init and __exit macros
#include <linux/io.h>           // For ioremap and MMIO access

#define DEVICE_NAME "my_uart"
#define UART_BASE  0xFE201000    // Base address of PL011 UART for Raspberry Pi 4
#define UART_DR    0x00          // Data Register offset
#define UART_FR    0x18          // Flag Register offset
#define UART_IBRD  0x24          // Integer Baud Rate Register
#define UART_FBRD  0x28          // Fractional Baud Rate Register
#define UART_LCRH  0x2C          // Line Control Register
#define UART_CR    0x30          // Control Register

static void __iomem *uart_base; // Memory-mapped base pointer for UART
int major;                      // Device major number

// Called when device is opened
static int my_uart_open(struct inode *inode, struct file *file)
{
    writel(0x0, uart_base + UART_CR);       // Disable UART

    // Set baud rate for 115200 with 48MHz UART clock
    writel(26, uart_base + UART_IBRD);      // Integer part
    writel(3, uart_base + UART_FBRD);       // Fractional part

    // 8 bits, no parity, 1 stop bit, enable FIFO
    writel((1 << 4) | (3 << 5), uart_base + UART_LCRH);

    // Enable UART, TX and RX
    writel((1 << 0) | (1 << 8) | (1 << 9), uart_base + UART_CR);

    printk(KERN_INFO "UART Initialized: 115200-8N1\n");
    return 0;
}

// Called when reading from the device
static ssize_t my_uart_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    size_t i;
    char kbuf[256];

    if (len > sizeof(kbuf)) len = sizeof(kbuf);  // Prevent buffer overflow

    for (i = 0; i < len; i++) {
        while (readl(uart_base + UART_FR) & (1 << 4)); // Wait until RXFE == 0 (not empty)
        kbuf[i] = readl(uart_base + UART_DR) & 0xFF;   // Read 1 byte
    }

    if (copy_to_user(buf, kbuf, len))                 // Copy to user-space
        return -EFAULT;

    return len;
}

// Called when writing to the device
static ssize_t my_uart_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    size_t i;
    char kbuf[256];

    if (len > sizeof(kbuf)) len = sizeof(kbuf);       // Limit length

    if (copy_from_user(kbuf, buf, len))               // Copy data from user-space
        return -EFAULT;

    for (i = 0; i < len; i++) {
        while (readl(uart_base + UART_FR) & (1 << 5)); // Wait until TXFF == 0 (not full)
        writel(kbuf[i], uart_base + UART_DR);         // Write byte to UART
    }

    return len;
}

// Called when device is closed
static int my_uart_release(struct inode *inode, struct file *file)
{
    return 0;
}

// File operations structure for this device
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_uart_open,
    .read = my_uart_read,
    .write = my_uart_write,
    .release = my_uart_release,
};

// Module initialization function
static int __init my_uart_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops); // Register character device
    if (major < 0) {
        pr_err("Failed to register char device\n");
        return major;
    }

    uart_base = ioremap(UART_BASE, 0x100);           // Map physical to virtual memory
    if (!uart_base) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to ioremap UART\n");
        return -ENOMEM;
    }

    pr_info("PL011 UART driver loaded. Major: %d\n", major);
    printk(KERN_INFO "Custom UART driver loaded\n");
    return 0;
}

// Module cleanup function
static void __exit my_uart_exit(void)
{
    iounmap(uart_base);                              // Unmap memory
    unregister_chrdev(240, DEVICE_NAME);             // Unregister device
    printk(KERN_INFO "Custom UART driver unloaded\n");
}

module_init(my_uart_init);     // Register init function
module_exit(my_uart_exit);     // Register exit function
MODULE_LICENSE("GPL");         // Required license macro

