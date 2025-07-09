#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/wait.h>

#define DEVICE_NAME "wait_q"

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;

static ssize_t my_read(struct file *filp, char __user *buf,
                       size_t len, loff_t *off)
{
    printk(KERN_INFO "mywait: Reader going to sleep... (flag=%d)\n", flag);

    wait_event_interruptible(wq, flag != 0);

    printk(KERN_INFO "mywait: Reader woke up. flag = %d\n", flag);

    flag = 0;

    if (copy_to_user(buf, "done\n", 5))
        return -EFAULT;

    return *off+5;
}

static ssize_t my_write(struct file *filp, const char __user *buf,
                        size_t len, loff_t *off)
{
    printk(KERN_INFO "mywait: Writer called. Waking up reader...\n");

    flag = 1;
    wake_up_interruptible(&wq);

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = my_read,
    .write = my_write,
};

static int __init waitq_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "mywait: Failed to allocate device number\n");
        return ret;
    }

    cdev_init(&my_cdev, &fops);
    ret = cdev_add(&my_cdev, dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev, 1);
        printk(KERN_ERR "mywait: Failed to add cdev\n");
        return ret;
    }

    // ✅ Updated for kernel 6.4+
    my_class = class_create(DEVICE_NAME);
    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        printk(KERN_ERR "mywait: Failed to create class\n");
        return PTR_ERR(my_class);
    }

    device_create(my_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "mywait: Module loaded\n");
    return 0;
}

static void __exit waitq_exit(void)
{
    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "mywait: Module unloaded\n");
}

module_init(waitq_init);
module_exit(waitq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("Wait queue example driver");

