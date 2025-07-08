// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include "mouse_ioctl.h"
#include <asm/io.h>

#define DRIVER_NAME "mouse_click_counter"
#define PS2_DATA_PORT 0x60        /* i8042 data register on PCs */
#define MOUSE_IRQ     12          /* PS/2 mouse IRQ line       */

static dev_t dev_num;
static struct cdev mouse_cdev;
static struct class *mouse_class;

static atomic_t click_count = ATOMIC_INIT(0);

/* ----------  IRQ handler  ---------- */
static irqreturn_t mouse_isr(int irq, void *dev_id)
{
    unsigned char data = inb(PS2_DATA_PORT);

    /* Bit 0 = Left button state; count only down‑transitions */
    static bool prev_left = false;
    bool left_now = data & 0x01;

    if (!prev_left && left_now)
        atomic_inc(&click_count);

    prev_left = left_now;
    return IRQ_HANDLED;
}

/* ----------  file_operations  ---------- */
static long mouse_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    unsigned long val;

    switch (cmd) {
    case IOCTL_GET_COUNT:
        val = (unsigned long)atomic_read(&click_count);
        return copy_to_user((unsigned long __user *)arg, &val, sizeof(val))
                   ? -EFAULT : 0;

    case IOCTL_CLR_COUNT:
        atomic_set(&click_count, 0);
        return 0;

    default:
        return -ENOTTY;
    }
}

static const struct file_operations mouse_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = mouse_ioctl,
    .compat_ioctl   = mouse_ioctl,
};

/* ----------  module init/exit  ---------- */
static int __init mouse_init(void)
{
    int ret;

    /* Allocate char‑dev numbers */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DRIVER_NAME);
    if (ret) return ret;

    /* Set up cdev */
    cdev_init(&mouse_cdev, &mouse_fops);
    mouse_cdev.owner = THIS_MODULE;
    ret = cdev_add(&mouse_cdev, dev_num, 1);
    if (ret) goto unregister;

    /* Create /dev/mouse_clicks */
    mouse_class = class_create(DRIVER_NAME);  // <- FIXED for kernel ≥ 6.4
    if (IS_ERR(mouse_class)) {
        ret = PTR_ERR(mouse_class);
        goto del_cdev;
    }

    device_create(mouse_class, NULL, dev_num, NULL, "mouse_clicks");

    /* Request shared IRQ 12 */
    ret = request_irq(MOUSE_IRQ, mouse_isr, IRQF_SHARED, DRIVER_NAME, &mouse_cdev);
    if (ret) goto destroy_dev;

    pr_info("%s loaded (major %d, minor %d)\n",
            DRIVER_NAME, MAJOR(dev_num), MINOR(dev_num));
    return 0;

destroy_dev:
    device_destroy(mouse_class, dev_num);
    class_destroy(mouse_class);
del_cdev:
    cdev_del(&mouse_cdev);
unregister:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

static void __exit mouse_exit(void)
{
    free_irq(MOUSE_IRQ, &mouse_cdev);
    device_destroy(mouse_class, dev_num);
    class_destroy(mouse_class);
    cdev_del(&mouse_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("%s unloaded\n", DRIVER_NAME);
}

module_init(mouse_init);
module_exit(mouse_exit);

MODULE_AUTHOR("Pritesh Sawant");
MODULE_DESCRIPTION("Mouse-click counter char driver with IOCTL (Linux 6.8+)");
MODULE_LICENSE("GPL");

