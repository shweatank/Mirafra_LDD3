#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include<linux/version.h>

#define DEVICE_NAME "mychar"

MODULE_LICENSE("GPL");

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;

static DECLARE_WAIT_QUEUE_HEAD(my_queue);
static int condition = 0;

// read() waits for condition or signal
static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    int ret;

    printk(KERN_INFO "mychar: read() called, waiting...\n");

    ret = wait_event_interruptible(my_queue, condition != 0);

    if (ret == -ERESTARTSYS) {
        printk(KERN_INFO "mychar: Interrupted by signal!\n");
        return -EINTR;
    }

    printk(KERN_INFO "mychar: Condition met, returning data\n");
    condition = 0;
    return 0;
}

// write() wakes up waiting readers
static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    printk(KERN_INFO "mychar: write() called, waking up readers\n");
    condition = 1;
    wake_up_interruptible(&my_queue);
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};

static int __init my_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev, 1);

    my_class = class_create(DEVICE_NAME);
    device_create(my_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "mychar: Module loaded\n");
    return 0;
}

static void __exit my_exit(void)
{
    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "mychar: Module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

