// x86_A_kernel.c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/io.h>

#define DEVICE_NAME "kbdnotify"
#define CLASS_NAME "kbdclass"
#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60

static struct class *kbd_class;
static struct cdev kbd_cdev;
static dev_t dev_num;
static char key_state = 0;

static irqreturn_t keyboard_irq(int irq, void *dev_id) {
    unsigned char scancode = inb(KBD_DATA_PORT);
    if (scancode == 0x1F) // 's'
        key_state = 's';
    else if (scancode == 0x10) // 'q'
        key_state = 'q';
    return IRQ_HANDLED;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    if (key_state == 0) return 0;
    if (copy_to_user(buf, &key_state, 1)) return -EFAULT;
    key_state = 0;
    return 1;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
};

static int __init kbd_init(void) {
    int ret;
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) return ret;

    cdev_init(&kbd_cdev, &fops);
    ret = cdev_add(&kbd_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    if (request_irq(KBD_IRQ, keyboard_irq, IRQF_SHARED, "kbd_irq", (void *)&kbd_cdev)) {
        cdev_del(&kbd_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -EBUSY;
    }

    kbd_class = class_create(CLASS_NAME);
    if (IS_ERR(kbd_class)) {
        free_irq(KBD_IRQ, (void *)&kbd_cdev);
        cdev_del(&kbd_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(kbd_class);
    }

    device_create(kbd_class, NULL, dev_num, NULL, DEVICE_NAME);
    pr_info("[kbdnotify] Module loaded\n");
    return 0;
}

static void __exit kbd_exit(void) {
    device_destroy(kbd_class, dev_num);
    class_destroy(kbd_class);
    free_irq(KBD_IRQ, (void *)&kbd_cdev);
    cdev_del(&kbd_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("[kbdnotify] Module unloaded\n");
}

module_init(kbd_init);
module_exit(kbd_exit);
MODULE_LICENSE("GPL");

