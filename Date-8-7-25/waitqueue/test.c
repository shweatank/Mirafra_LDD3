#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define DEVICE_NAME "mywait"
struct calc_data {
    int a;
    int b;
    char op;
    int result;
};
static struct calc_data data;
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha");
MODULE_DESCRIPTION("Waitqueue Example Module");

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;  

// Condition for waking up

// --- Read blocks until flag is set ---
static ssize_t my_read(struct file *filp, char __user *buf,size_t len, loff_t *off)
{
    printk(KERN_INFO "mywait: Reader going to sleep...\n");

    // Wait until flag is set
    wait_event_interruptible(wq, flag != 0);

    printk(KERN_INFO "mywait: Reader woke up, flag = %d\n", flag);

    // Clear flag and return to user
    if (*off > 0)
        return 0; 
     if (copy_to_user(buf, &data, sizeof(data)))
        return -EFAULT;

    flag = 0;
    return sizeof(data);
}

// --- Write sets flag and wakes up reader ---
static ssize_t my_write(struct file *filp, const char __user *buf,size_t len, loff_t *off)
{
        if (copy_from_user(&data, buf, sizeof(data)))
        return -EFAULT;

    printk(KERN_INFO "mywait: Received a=%d, b=%d, op=%c\n", data.a, data.b, data.op);

    switch (data.op) {
        case '+': data.result = data.a + data.b; break;
        case '-': data.result = data.a - data.b; break;
        case '*': data.result = data.a * data.b; break;
        case '/':
            if (data.b != 0)
                data.result = data.a / data.b;
            else {
                printk(KERN_ERR "mywait: Divide by zero\n");
                data.result = 0;
            }
            break;
        default:
            printk(KERN_ERR "mywait: Invalid operator\n");
            data.result = 0;
    }
    printk(KERN_INFO "mywait: Writer waking up reader\n");
    flag = 1;
    wake_up_interruptible(&wq);
    return sizeof(data);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};

static int __init waitq_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev, 1);

    my_class = class_create(DEVICE_NAME);
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

