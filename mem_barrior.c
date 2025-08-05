// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/completion.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <asm/barrier.h>  // smp_wmb(), smp_rmb()

#define DEVICE_NAME "sync_char"
#define CLASS_NAME  "sync_class"
#define BUF_SIZE 128

static int major;
static struct class *sync_class;
static struct device *sync_device;
static struct cdev sync_cdev;

static struct task_struct *consumer_thread;
static DECLARE_COMPLETION(data_ready);
static DEFINE_MUTEX(sync_lock);

static char *shared_data;
static int data_flag = 0;

static int consumer_fn(void *arg)
{
    while (!kthread_should_stop()) {
        wait_for_completion_interruptible(&data_ready);
        smp_rmb();  // read memory barrier

        if (data_flag) {
            pr_info("RPi4B-Consumer: Received: %s\n", shared_data);
            data_flag = 0;
        }
        msleep(100);
    }
    return 0;
}

static ssize_t sync_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *ppos)
{
    if (count >= BUF_SIZE) count = BUF_SIZE - 1;

    mutex_lock(&sync_lock);
    if (copy_from_user(shared_data, buf, count)) {
        mutex_unlock(&sync_lock);
        return -EFAULT;
    }
    shared_data[count] = '\0';

    smp_wmb();  // write memory barrier
    data_flag = 1;

    complete(&data_ready);
    mutex_unlock(&sync_lock);
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = sync_write,
};

static int __init sync_init(void)
{
    dev_t dev;

    shared_data = kzalloc(BUF_SIZE, GFP_KERNEL);
    if (!shared_data)
        return -ENOMEM;

    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME)) {
        kfree(shared_data);
        return -1;
    }

    major = MAJOR(dev);
    cdev_init(&sync_cdev, &fops);
    if (cdev_add(&sync_cdev, dev, 1)) {
        unregister_chrdev_region(dev, 1);
        kfree(shared_data);
        return -1;
    }

    sync_class = class_create(CLASS_NAME);
    if (IS_ERR(sync_class)) {
        cdev_del(&sync_cdev);
        unregister_chrdev_region(dev, 1);
        kfree(shared_data);
        return PTR_ERR(sync_class);
    }

    sync_device = device_create(sync_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(sync_device)) {
        class_destroy(sync_class);
        cdev_del(&sync_cdev);
        unregister_chrdev_region(dev, 1);
        kfree(shared_data);
        return PTR_ERR(sync_device);
    }

    consumer_thread = kthread_run(consumer_fn, NULL, "consumer_thread");
    if (IS_ERR(consumer_thread)) {
        device_destroy(sync_class, dev);
        class_destroy(sync_class);
        cdev_del(&sync_cdev);
        unregister_chrdev_region(dev, 1);
        kfree(shared_data);
        return PTR_ERR(consumer_thread);
    }

    pr_info("RPi4B: sync_char module loaded\n");
    return 0;
}

static void __exit sync_exit(void)
{
    kthread_stop(consumer_thread);
    device_destroy(sync_class, MKDEV(major, 0));
    class_destroy(sync_class);
    cdev_del(&sync_cdev);
    unregister_chrdev_region(MKDEV(major, 0), 1);
    kfree(shared_data);
    pr_info("RPi4B: sync_char module unloaded\n");
}

module_init(sync_init);
module_exit(sync_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba");
MODULE_DESCRIPTION("RPi4B: Sync Char Driver with Completion and Barriers");
MODULE_VERSION("1.0");
