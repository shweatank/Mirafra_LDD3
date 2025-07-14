// select_driver.c
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/wait.h>

#define DEVICE_NAME "select_dev"
#define BUF_SIZE 128

static dev_t dev_num;
static struct cdev my_cdev;
static char device_buffer[BUF_SIZE];
static int data_available = 0;

static DECLARE_WAIT_QUEUE_HEAD(wq);

static ssize_t select_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    if (!data_available)
        return 0;

    if (copy_to_user(buf, device_buffer, BUF_SIZE))
        return -EFAULT;

    data_available = 0; // Clear flag after reading
    return BUF_SIZE;
}

static ssize_t select_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    if (len > BUF_SIZE)
        len = BUF_SIZE;

    if (copy_from_user(device_buffer, buf, len))
        return -EFAULT;

    data_available = 1;
    wake_up_interruptible(&wq);  // Wake up processes waiting in select()
    return len;
}

static unsigned int select_poll(struct file *file, struct poll_table_struct *wait) {
    unsigned int mask = 0;

    poll_wait(file, &wq, wait);  // Register the wait queue

    if (data_available)
        mask |= POLLIN | POLLRDNORM;  // Data is available for reading

    return mask;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = select_read,
    .write = select_write,
    .poll = select_poll,  // Required for select()/poll()
};

static int __init select_driver_init(void) {
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev_num, 1);
    pr_info("select driver loaded: major=%d minor=%d\n", MAJOR(dev_num), MINOR(dev_num));
    return 0;
}

static void __exit select_driver_exit(void) {
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("select driver unloaded\n");
}

module_init(select_driver_init);
module_exit(select_driver_exit);
MODULE_LICENSE("GPL");

