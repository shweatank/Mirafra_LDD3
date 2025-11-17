#include <linux/module.h>        // For module macros
#include <linux/kernel.h>        // For kernel info macros
#include <linux/fs.h>            // For file operations structure
#include <linux/init.h>          // For __init and __exit macros
#include <linux/uaccess.h>       // For copy_to_user()
#include <linux/io.h>            // For ioremap, iounmap, readl, writel
#include <linux/cdev.h>          // For cdev APIs
#include <linux/device.h>        // For device_create, class_create
#include <linux/delay.h>         // For msleep()

// Device and memory definitions
#define DEVICE_NAME "uart_rx"
#define UART0_BASE  0xFE201000   // Physical base address of UART0 on Raspberry Pi 4
#define UART_DR     0x00         // Data Register offset
#define UART_FR     0x18         // Flag Register offset
#define UART_CR     0x30         // Control Register offset

#define UART_FR_RXFE (1 << 4)    // RX FIFO Empty flag bit

static void __iomem *uart_base;  // Virtual base pointer for UART after ioremap
static int major;                // Major number for character device
static struct cdev uart_cdev;    // Character device structure
static struct class *uart_class; // Pointer to device class
static struct device *uart_device; // Pointer to device node

#define UART_BUFFER_SIZE 128
static char uart_buffer[UART_BUFFER_SIZE]; // Buffer to hold received UART characters
static int uart_buf_index = 0;             // Buffer index

// UART read operation - reads characters from UART and passes to user
static ssize_t uart_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    char ch;

    uart_buf_index = 0; // Reset buffer index

    // Wait until there is data in the UART RX FIFO
    while (readl(uart_base + UART_FR) & UART_FR_RXFE) {
        msleep(10);  // Sleep for a short time to prevent busy waiting
    }

    // Read characters from UART until RX FIFO is empty
    while (!(readl(uart_base + UART_FR) & UART_FR_RXFE)) {
        ch = readl(uart_base + UART_DR) & 0xFF; // Read single byte from data register

        if (uart_buf_index < UART_BUFFER_SIZE - 1)
            uart_buffer[uart_buf_index++] = ch; // Store character in buffer
        else
            break; // Prevent buffer overflow
    }

    uart_buffer[uart_buf_index] = '\0'; // Null-terminate the buffer

    // Copy buffer to user space
    if (copy_to_user(buf, uart_buffer, uart_buf_index))
        return -EFAULT;

    return uart_buf_index; // Return number of bytes copied
}

// File operations structure for character device
static struct file_operations uart_fops = {
    .owner = THIS_MODULE,
    .read  = uart_read, // Only read is implemented
};

// Module initialization
static int __init uart_rx_init(void)
{
    dev_t dev;

    pr_info("UART RX Driver Init\n");

    // Allocate character device numbers
    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0)
        return -1;

    major = MAJOR(dev); // Store major number
    cdev_init(&uart_cdev, &uart_fops); // Initialize character device with file ops

    // Add character device to the system
    if (cdev_add(&uart_cdev, dev, 1) < 0) {
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    // Create device class
    uart_class = class_create(DEVICE_NAME);
    if (IS_ERR(uart_class)) {
        pr_err("Failed to create class\n");
        cdev_del(&uart_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(uart_class);
    }

    // Create device node /dev/uart_rx
    uart_device = device_create(uart_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(uart_device)) {
        pr_err("Failed to create device\n");
        class_destroy(uart_class);
        cdev_del(&uart_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(uart_device);
    }

    // Map UART0 memory region into kernel virtual address space
    uart_base = ioremap(UART0_BASE, 0x100); 
    if (!uart_base) {
        pr_err("UART ioremap failed\n");
        device_destroy(uart_class, dev);
        class_destroy(uart_class);
        cdev_del(&uart_cdev);
        unregister_chrdev_region(dev, 1);
        return -ENOMEM;
    }

    // Enable UART RX (bits: UARTEN = 1 << 0, RXE = 1 << 9)
    writel((1 << 0) | (1 << 9), uart_base + UART_CR);

    pr_info("UART RX device ready: /dev/%s (major %d)\n", DEVICE_NAME, major);
    return 0;
}

// Module cleanup
static void __exit uart_rx_exit(void)
{
    dev_t dev = MKDEV(major, 0);

    pr_info("UART RX Driver Exit\n");

    // Disable UART
    writel(0x0, uart_base + UART_CR);

    // Unmap memory
    iounmap(uart_base);

    // Cleanup character device
    device_destroy(uart_class, dev);
    class_destroy(uart_class);
    cdev_del(&uart_cdev);
    unregister_chrdev_region(dev, 1);
}

// Module entry and exit points
module_init(uart_rx_init);
module_exit(uart_rx_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team 2");
MODULE_DESCRIPTION("Custom UART driver code for RPI4");

