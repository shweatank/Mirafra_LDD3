#include <linux/module.h>          // Core header for loading LKMs into the kernel
#include <linux/kernel.h>          // For KERN_INFO and other macros
#include <linux/init.h>            // For __init and __exit macros
#include <linux/io.h>              // For ioremap, iounmap, and memory-mapped I/O
#include <linux/kthread.h>         // For kernel threads
#include <linux/delay.h>           // For msleep
#include <linux/fs.h>              // For file operations
#include <linux/device.h>          // For device creation
#include <linux/uaccess.h>         // For copy_to_user, copy_from_user
#include <linux/cdev.h>            // For character device structures
#include <linux/ioctl.h>           // For ioctl calls
#include <linux/i2c.h>             // For I2C communication
#include <linux/slab.h>            // For kmalloc, kfree
#include <linux/wait.h>            // For wait queues
#include <linux/mutex.h>           // For mutual exclusion

// Macros and hardware constants

#define DEVICE_NAME "custom_uart_game"
#define UART0_BASE  0xFE201000       // UART0 base physical address on Raspberry Pi 4
#define UART_DR     0x00             // Data Register
#define UART_FR     0x18             // Flag Register
#define UART_IBRD   0x24             // Integer Baud Rate
#define UART_FBRD   0x28             // Fractional Baud Rate
#define UART_LCRH   0x2C             // Line Control
#define UART_CR     0x30             // Control Register
#define UART_IMSC   0x38             // Interrupt Mask Set/Clear

#define OLED_I2C_ADDR 0x3C           // I2C address of the SSD1306 OLED
#define BUF_SIZE 128

// Global variables

static void __iomem *uart_base;              // Pointer to memory-mapped UART registers
static struct task_struct *rx_thread;        // Kernel thread for UART reception
static int major;                            // Major number for character device
static struct class *uart_class;             // Device class
static struct device *uart_device;           // Device struct

static struct i2c_adapter *i2c_adap;         // I2C adapter (i2c-1)
static struct i2c_client *oled_client;       // I2C client for SSD1306

static char received_uart_char = 0;          // Last received UART character
static char user_guess_char = -1;            // User-provided guess

// Basic 5x7 font table for characters 0–9
static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x5F,0x00,0x00}, ...
};

// Wait queue and mutex for UART read synchronization
static DECLARE_WAIT_QUEUE_HEAD(read_queue);
static char data_buffer[BUF_SIZE];           // Shared buffer for output
static int data_ready = 0;                   // Flag to indicate new data available
static DEFINE_MUTEX(data_mutex);             // Protects access to shared buffer

// SSD1306 I2C OLED Functions

// Write a command to the OLED
static int ssd1306_write_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd}; // 0x00 = command mode
    struct i2c_msg msg = {
        .addr = oled_client->addr,
        .flags = 0,           // Write
        .len = 2,
        .buf = buf
    };
    return i2c_transfer(oled_client->adapter, &msg, 1);
}

// Write data (pixels) to OLED
static int ssd1306_write_data(uint8_t *data, size_t len) {
    uint8_t *buf = kmalloc(len + 1, GFP_KERNEL);
    if (!buf) return -ENOMEM;
    buf[0] = 0x40; // 0x40 = data mode
    memcpy(buf + 1, data, len);
    struct i2c_msg msg = {
        .addr = oled_client->addr,
        .flags = 0,
        .len = len + 1,
        .buf = buf
    };
    int ret = i2c_transfer(oled_client->adapter, &msg, 1);
    kfree(buf);
    return ret;
}

// Clear the entire OLED screen
static void ssd1306_clear_display(void) {
    for (int page = 0; page < 8; page++) {
        ssd1306_write_cmd(0xB0 + page);  // Set page
        ssd1306_write_cmd(0x00);         // Column low nibble
        ssd1306_write_cmd(0x10);         // Column high nibble
        uint8_t clear[128] = {0};        // 128 columns of 0
        ssd1306_write_data(clear, 128);
    }
}

// Display a string at a given page and column
static void ssd1306_display_string(const char *str, uint8_t page, uint8_t col) {
    while (*str) {
        uint8_t c = *str++;
        if (c < 32 || c > 126) c = '?';
        ssd1306_write_cmd(0xB0 + page);
        ssd1306_write_cmd(0x00 + (col & 0x0F));
        ssd1306_write_cmd(0x10 + ((col >> 4) & 0x0F));
        uint8_t buffer[6];
        memcpy(buffer, font5x7[c - 32], 5);
        buffer[5] = 0x00;
        ssd1306_write_data(buffer, 6);
        col += 6;
    }
}

// Initialize OLED (SSD1306-specific command sequence)
static void ssd1306_init_display(void) {
    ssd1306_write_cmd(0xAE); // Display off
    ssd1306_write_cmd(0xD5); ssd1306_write_cmd(0x80);
    ...
    ssd1306_write_cmd(0xAF); // Display on
    ssd1306_clear_display(); // Clear after init
}

// UART Functions (PL011 Registers)

// Send one character
static void uart_send_char(char c) {
    while (readl(uart_base + UART_FR) & (1 << 5));  // Wait while TX FIFO is full
    writel(c, uart_base + UART_DR);
}

// Send a string character by character
static void uart_send_string(const char *s) {
    while (*s) uart_send_char(*s++);
}

// Receive one character
static char uart_recv_char(void) {
    return readl(uart_base + UART_DR) & 0xFF; // Mask only 8 bits
}


// Kernel thread to read UART in background
static int uart_rx_thread(void *data) {
    char c;

    while (!kthread_should_stop()) {
        // Check if RX FIFO is not empty
        if (!(readl(uart_base + UART_FR) & (1 << 4))) {
            c = uart_recv_char();
            received_uart_char = c;

            mutex_lock(&data_mutex);
            snprintf(data_buffer, BUF_SIZE, "Received: %c\n", c);
            data_ready = 1;
            mutex_unlock(&data_mutex);
            wake_up_interruptible(&read_queue);  // Wake up any waiting read()

            ssd1306_clear_display();
            if (user_guess_char != -1) {
                if (user_guess_char == received_uart_char) {
                    ssd1306_display_string("WON", 0, 0);
                    mutex_lock(&data_mutex);
                    snprintf(data_buffer, BUF_SIZE, "Result: WON\n");
                } else {
                    ssd1306_display_string("LOST", 0, 0);
                    mutex_lock(&data_mutex);
                    snprintf(data_buffer, BUF_SIZE, "Result: LOST\n");
                }
                data_ready = 1;
                mutex_unlock(&data_mutex);
                wake_up_interruptible(&read_queue);
                user_guess_char = -1;
            }
        }
        msleep(100); // Sleep to reduce CPU usage
    }
    return 0;
}

// Character device file operations

static ssize_t uart_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    char kbuf[BUF_SIZE] = {0};
    if (count > BUF_SIZE - 1) count = BUF_SIZE - 1;
    if (copy_from_user(kbuf, buf, count)) return -EFAULT;

    if (kbuf[0] >= '0' && kbuf[0] <= '9') {
        user_guess_char = kbuf[0];
    } else {
        return -EINVAL;
    }
    return count;
}

static ssize_t uart_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int ret;
    if (wait_event_interruptible(read_queue, data_ready))
        return -ERESTARTSYS;

    mutex_lock(&data_mutex);
    if (count > strlen(data_buffer)) count = strlen(data_buffer);
    ret = copy_to_user(buf, data_buffer, count);
    if (ret) {
        mutex_unlock(&data_mutex);
        return -EFAULT;
    }
    data_ready = 0;
    mutex_unlock(&data_mutex);

    return count;
}

static int uart_open(struct inode *inode, struct file *file) { return 0; }
static int uart_release(struct inode *inode, struct file *file) { return 0; }

// File operations table
static struct file_operations uart_fops = {
    .owner = THIS_MODULE,
    .write = uart_write,
    .read = uart_read,
    .open = uart_open,
    .release = uart_release,
};

// Module Init and Exit

static int __init uart_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &uart_fops);
    uart_class = class_create(DEVICE_NAME);
    uart_device = device_create(uart_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    uart_base = ioremap(UART0_BASE, 0x1000);
    if (!uart_base) return -ENOMEM;

    // UART setup
    writel(0x0, uart_base + UART_CR);               // Disable UART
    writel(26, uart_base + UART_IBRD);              // Integer Baud Rate for 115200
    writel(3, uart_base + UART_FBRD);               // Fractional Baud Rate
    writel((3 << 5) | (1 << 4), uart_base + UART_LCRH); // 8N1, FIFO
    writel(0x0, uart_base + UART_IMSC);             // Disable interrupts
    writel((1 << 0) | (1 << 8) | (1 << 9), uart_base + UART_CR); // Enable UART, TX, RX

    i2c_adap = i2c_get_adapter(1);
    if (!i2c_adap) return -ENODEV;

    oled_client = i2c_new_dummy_device(i2c_adap, OLED_I2C_ADDR);
    if (!oled_client) {
        i2c_put_adapter(i2c_adap);
        return -ENODEV;
    }

    ssd1306_init_display();

    rx_thread = kthread_run(uart_rx_thread, NULL, "uart_rx_thread");
    if (IS_ERR(rx_thread)) {
        i2c_unregister_device(oled_client);
        i2c_put_adapter(i2c_adap);
        return PTR_ERR(rx_thread);
    }

    pr_info("Custom UART game driver loaded\n");
    return 0;
}

static void __exit uart_exit(void) {
    if (rx_thread) kthread_stop(rx_thread);
    ssd1306_clear_display();
    i2c_unregister_device(oled_client);
    i2c_put_adapter(i2c_adap);
    if (uart_base) iounmap(uart_base);
    device_destroy(uart_class, MKDEV(major, 0));
    class_destroy(uart_class);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Custom UART game driver unloaded\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Batch 4");
MODULE_DESCRIPTION("Custom UART game kernel module with read support");

