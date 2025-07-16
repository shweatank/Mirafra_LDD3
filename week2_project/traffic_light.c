#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include "traffic_light_ioctl.h"

#define DEVICE_NAME "traffic_light_irq"
#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60

enum light_state { RED, GREEN, YELLOW };
static enum light_state current_state = RED;
static bool irq_enabled = false;

static dev_t dev_num;
static struct cdev tl_cdev;

static irqreturn_t kbd_irq_handler(int irq, void *dev_id) {
    if (!irq_enabled)
        return IRQ_NONE;

    unsigned char scancode = inb(KBD_DATA_PORT); // Read scan code
    (void)scancode; // Not used

    // Cycle to next state
    current_state = (current_state + 1) % 3;
    printk(KERN_INFO "Traffic Light changed to: %s\n",
           current_state == RED ? "RED" :
           current_state == GREEN ? "GREEN" : "YELLOW");

    return IRQ_HANDLED;
}

static long tl_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case TL_SET_RED:
            current_state = RED;
            irq_enabled = false;
            printk(KERN_INFO "Manual Set: RED\n");
            break;
        case TL_SET_GREEN:
            current_state = GREEN;
            irq_enabled = false;
            printk(KERN_INFO "Manual Set: GREEN\n");
            break;
        case TL_SET_YELLOW:
            current_state = YELLOW;
            irq_enabled = false;
            printk(KERN_INFO "Manual Set: YELLOW\n");
            break;
        case TL_IRQ_ENABLE:
            irq_enabled = true;
            printk(KERN_INFO "Keyboard IRQ-based light change ENABLED\n");
            break;
        case TL_IRQ_DISABLE:
            irq_enabled = false;
            printk(KERN_INFO "Keyboard IRQ-based light change DISABLED\n");
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static ssize_t tl_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    char msg[16];
    int len = snprintf(msg, sizeof(msg), "%s\n",
        current_state == RED ? "RED" :
        current_state == GREEN ? "GREEN" : "YELLOW");

    return simple_read_from_buffer(buf, count, ppos, msg, len);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = tl_ioctl,
    .read = tl_read,
};

static int __init tl_init(void) {
    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME))
        return -1;

    cdev_init(&tl_cdev, &fops);
    if (cdev_add(&tl_cdev, dev_num, 1)) {
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    if (request_irq(KBD_IRQ, kbd_irq_handler, IRQF_SHARED, DEVICE_NAME, (void *)(kbd_irq_handler))) {
        cdev_del(&tl_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    printk(KERN_INFO "Traffic Light IRQ Module loaded: Major = %d\n", MAJOR(dev_num));
    return 0;
}

static void __exit tl_exit(void) {
    free_irq(KBD_IRQ, (void *)(kbd_irq_handler));
    cdev_del(&tl_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "Traffic Light IRQ Module unloaded\n");
}

module_init(tl_init);
module_exit(tl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("adi narayana");
MODULE_DESCRIPTION("Virtual Traffic Light using Keyboard IRQ");

