// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/atomic.h>

#define DEVICE_NAME "mydev"
#define CLASS_NAME  "multi_dev"
#define MAX_DEVICES 4
#define BUF_SIZE 128

struct my_device {
    struct cdev cdev;
    struct device *dev;
    struct mutex lock;
    atomic_t in_use;
    int device_id;
    char buffer[BUF_SIZE];
};

static dev_t dev_num;
static struct class *multi_class;
static struct my_device devices[MAX_DEVICES];

static int my_open(struct inode *inode, struct file *filp)
{
    int minor = iminor(inode);
    struct my_device *dev = &devices[minor];

    // Exclusive access check
    if (!atomic_dec_and_test(&dev->in_use)) {
        atomic_inc(&dev->in_use);
        return -EBUSY;
    }

    filp->private_data = dev;
    return 0;
}

static int my_release(struct inode *inode, struct file *filp)
{
    struct my_device *dev = filp->private_data;
    atomic_inc(&dev->in_use);
    return 0;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    struct my_device *dev = filp->private_data;

    if (count >= BUF_SIZE)
        count = BUF_SIZE - 1;

    mutex_lock(&dev->lock);
    if (copy_from_user(dev->buffer, buf, count)) {
        mutex_unlock(&dev->lock);
        return -EFAULT;
    }
    dev->buffer[count] = '\0';
    mutex_unlock(&dev->lock);

    pr_info("Device %d wrote: %s\n", dev->device_id, dev->buffer);
    return count;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = my_open,
    .release = my_release,
    .write   = my_write,
};

static int __init multi_dev_init(void)
{
    int i;
    int ret;
    dev_t dev_base;

    ret = alloc_chrdev_region(&dev_num, 0, MAX_DEVICES, DEVICE_NAME);
    if (ret)
        return ret;

    multi_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(multi_class)) {
        unregister_chrdev_region(dev_num, MAX_DEVICES);
        return PTR_ERR(multi_class);
    }

    dev_base = dev_num;

    for (i = 0; i < MAX_DEVICES; i++) {
        struct my_device *dev = &devices[i];
        dev->device_id = i;
        mutex_init(&dev->lock);
        atomic_set(&dev->in_use, 1);  // Initially available

        cdev_init(&dev->cdev, &fops);
        dev->cdev.owner = THIS_MODULE;

        ret = cdev_add(&dev->cdev, dev_base + i, 1);
        if (ret)
            goto fail;

        dev->dev = device_create(multi_class, NULL, dev_base + i, NULL, DEVICE_NAME "%d", i);
        if (IS_ERR(dev->dev)) {
            cdev_del(&dev->cdev);
            goto fail;
        }
    }

    pr_info("Multi-device driver loaded\n");
    return 0;

fail:
    while (--i >= 0) {
        device_destroy(multi_class, dev_base + i);
        cdev_del(&devices[i].cdev);
    }
    class_destroy(multi_class);
    unregister_chrdev_region(dev_num, MAX_DEVICES);
    return ret;
}

static void __exit multi_dev_exit(void)
{
    int i;

    for (i = 0; i < MAX_DEVICES; i++) {
        device_destroy(multi_class, dev_num + i);
        cdev_del(&devices[i].cdev);
    }

    class_destroy(multi_class);
    unregister_chrdev_region(dev_num, MAX_DEVICES);

    pr_info("Multi-device driver unloaded\n");
}

module_init(multi_dev_init);
module_exit(multi_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant Soni");
MODULE_DESCRIPTION("Multi-device char driver with exclusive access for RPi 4B");
MODULE_VERSION("1.0");
