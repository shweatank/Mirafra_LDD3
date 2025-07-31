#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/mutex.h>

#define DEVICE_NAME "custom_uart_game"
#define UART0_BASE 0xFE201000
#define UART_DR    0x00
#define UART_FR    0x18
#define UART_IBRD  0x24
#define UART_FBRD  0x28
#define UART_LCRH  0x2C
#define UART_CR    0x30
#define UART_IMSC  0x38

#define OLED_I2C_ADDR 0x3C
#define BUF_SIZE 128

static void __iomem *uart_base;
static struct task_struct *rx_thread;
static int major;
static struct class *uart_class;
static struct device *uart_device;

static struct i2c_adapter *i2c_adap;
static struct i2c_client *oled_client;

static char received_uart_char = 0;
static char user_guess_char = -1;

static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x5F,0x00,0x00},{0x00,0x07,0x00,0x07,0x00},
    {0x14,0x7F,0x14,0x7F,0x14},{0x24,0x2A,0x7F,0x2A,0x12},{0x23,0x13,0x08,0x64,0x62},
    {0x36,0x49,0x55,0x22,0x50},{0x00,0x05,0x03,0x00,0x00},{0x00,0x1C,0x22,0x41,0x00},
    {0x00,0x41,0x22,0x1C,0x00},{0x14,0x08,0x3E,0x08,0x14},{0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00},{0x08,0x08,0x08,0x08,0x08},{0x00,0x60,0x60,0x00,0x00},
    {0x20,0x10,0x08,0x04,0x02},{0x3E,0x51,0x49,0x45,0x3E},{0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4B,0x31},{0x18,0x14,0x12,0x7F,0x10},
    {0x27,0x45,0x45,0x45,0x39},{0x3C,0x4A,0x49,0x49,0x30},{0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36},{0x06,0x49,0x49,0x29,0x1E}
};

static DECLARE_WAIT_QUEUE_HEAD(read_queue);
static char data_buffer[BUF_SIZE];
static int data_ready = 0;
static DEFINE_MUTEX(data_mutex);

static int ssd1306_write_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    struct i2c_msg msg = {.addr = oled_client->addr, .flags = 0, .len = 2, .buf = buf};
    return i2c_transfer(oled_client->adapter, &msg, 1);
}

static int ssd1306_write_data(uint8_t *data, size_t len) {
    uint8_t *buf = kmalloc(len + 1, GFP_KERNEL);
    if (!buf) return -ENOMEM;
    buf[0] = 0x40;
    memcpy(buf + 1, data, len);
    struct i2c_msg msg = {.addr = oled_client->addr, .flags = 0, .len = len + 1, .buf = buf};
    int ret = i2c_transfer(oled_client->adapter, &msg, 1);
    kfree(buf);
    return ret;
}

static void ssd1306_clear_display(void) {
    for (int page = 0; page < 8; page++) {
        ssd1306_write_cmd(0xB0 + page);
        ssd1306_write_cmd(0x00);
        ssd1306_write_cmd(0x10);
        uint8_t clear[128] = {0};
        ssd1306_write_data(clear, 128);
    }
}

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

static void ssd1306_init_display(void) {
    ssd1306_write_cmd(0xAE); ssd1306_write_cmd(0xD5); ssd1306_write_cmd(0x80);
    ssd1306_write_cmd(0xA8); ssd1306_write_cmd(0x3F);
    ssd1306_write_cmd(0xD3); ssd1306_write_cmd(0x00);
    ssd1306_write_cmd(0x40); ssd1306_write_cmd(0x8D); ssd1306_write_cmd(0x14);
    ssd1306_write_cmd(0x20); ssd1306_write_cmd(0x00);
    ssd1306_write_cmd(0xA1); ssd1306_write_cmd(0xC8);
    ssd1306_write_cmd(0xDA); ssd1306_write_cmd(0x12);
    ssd1306_write_cmd(0x81); ssd1306_write_cmd(0xCF);
    ssd1306_write_cmd(0xD9); ssd1306_write_cmd(0xF1);
    ssd1306_write_cmd(0xDB); ssd1306_write_cmd(0x40);
    ssd1306_write_cmd(0xA4); ssd1306_write_cmd(0xA6); ssd1306_write_cmd(0xAF);
    ssd1306_clear_display();
}

static void uart_send_char(char c) {
    pr_info("send----\n");
    while (readl(uart_base + UART_FR) & (1 << 5));
    pr_info("RESA--\n");
    writel(c, uart_base + UART_DR);
    pr_info("WRITTEN---\n");
}

static void uart_send_string(const char *s) {
    
pr_info("semding dtring");	
    while (*s) uart_send_char(*s++);
    pr_info("string\n");
}

static char uart_recv_char(void) {

    pr_info("RECC---\n");
    readl(uart_base + UART_DR) & 0xFF;
    pr_info("prineted dtata:%c\n",uart_base+UART_DR);
    return readl(uart_base+UART_DR) & 0XFF;
}

static int uart_rx_thread(void *data) {
    char c;

    while (!kthread_should_stop()) {
        if (!(readl(uart_base + UART_FR) & (1 << 4))) {
            c = uart_recv_char();
            received_uart_char = c;

            mutex_lock(&data_mutex);
            snprintf(data_buffer, BUF_SIZE, "Received: %c\n", c);
            data_ready = 1;
            mutex_unlock(&data_mutex);

            wake_up_interruptible(&read_queue);

            ssd1306_clear_display();
            if (user_guess_char != -1) {
                if (user_guess_char == received_uart_char) {
                    ssd1306_display_string("WON", 0, 0);
                    mutex_lock(&data_mutex);
                    snprintf(data_buffer, BUF_SIZE, "Result: WON\n");
                    data_ready = 1;
                    mutex_unlock(&data_mutex);
                    wake_up_interruptible(&read_queue);
                } else {
                    ssd1306_display_string("LOST", 0, 0);
                    mutex_lock(&data_mutex);
                    snprintf(data_buffer, BUF_SIZE, "Result: LOST\n");
                    data_ready = 1;
                    mutex_unlock(&data_mutex);
                    wake_up_interruptible(&read_queue);
                }
                user_guess_char = -1; // reset after match attempt
            }
        }
        msleep(100);
    }
    return 0;
}

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
    if (count > strlen(data_buffer))
        count = strlen(data_buffer);

    ret = copy_to_user(buf, data_buffer, count);
    if (ret) {
        mutex_unlock(&data_mutex);
        return -EFAULT;
    }
    data_ready = 0;
    mutex_unlock(&data_mutex);

    return count;
}

static int uart_open(struct inode *inode, struct file *file) {
    return 0;
}

static int uart_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations uart_fops = {
    .owner = THIS_MODULE,
    .write = uart_write,
    .read = uart_read,
    .open = uart_open,
    .release = uart_release,
};

static int __init uart_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &uart_fops);
    uart_class = class_create(DEVICE_NAME);
    uart_device = device_create(uart_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    uart_base = ioremap(UART0_BASE, 0x1000);
    if (!uart_base) {
        pr_err("Failed to ioremap UART\n");
        return -ENOMEM;
    }

    // UART initialization
    writel(0x0, uart_base + UART_CR);
    writel(26, uart_base + UART_IBRD);  // for baud 115200 on RPi
    writel(3, uart_base + UART_FBRD);
    writel((3 << 5) | (1 << 4), uart_base + UART_LCRH);  // 8-bit, FIFO enable
    writel(0x0, uart_base + UART_IMSC);
    writel((1 << 0) | (1 << 8) | (1 << 9), uart_base + UART_CR);

    // Setup I2C for OLED (usually i2c-1 on Raspberry Pi)
    i2c_adap = i2c_get_adapter(1);
    if (!i2c_adap) {
        pr_err("Failed to get I2C adapter\n");
        return -ENODEV;
    }

    oled_client = i2c_new_dummy_device(i2c_adap, OLED_I2C_ADDR);
    if (!oled_client) {
        pr_err("Failed to create I2C client for OLED\n");
        i2c_put_adapter(i2c_adap);
        return -ENODEV;
    }

    ssd1306_init_display();

    rx_thread = kthread_run(uart_rx_thread, NULL, "uart_rx_thread");
    if (IS_ERR(rx_thread)) {
        pr_err("Failed to create kernel thread\n");
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
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Custom UART game kernel module with read support");

