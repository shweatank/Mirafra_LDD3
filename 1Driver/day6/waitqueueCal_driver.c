#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define DEVICE_NAME "calcwait"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Waitqueue Calculator Module");

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;

static char result_buf[100];  // To store result string

// --- Reader blocks until result is ready ---
static ssize_t my_read(struct file *filp, char __user *buf,
                       size_t len, loff_t *off)
{
    printk(KERN_INFO "calcwait: Reader sleeping...\n");

    wait_event_interruptible(wq, flag != 0);

    printk(KERN_INFO "calcwait: Reader woke up. Sending result: %s\n", result_buf);

    flag = 0;

    if (copy_to_user(buf, result_buf, strlen(result_buf)))
        return -EFAULT;

    return strlen(result_buf);
}

// --- Writer sends expression, triggers computation ---
static ssize_t my_write(struct file *filp, const char __user *buf,
                        size_t len, loff_t *off)
{
    char input[100];
    int a, b, res;
    char op;

    if (len >= sizeof(input))
        return -EINVAL;

    if (copy_from_user(input, buf, len))
        return -EFAULT;

    input[len] = '\0';  // Null-terminate

    // Expected format: "5 + 3"
    if (sscanf(input, "%d %c %d", &a, &op, &b) != 3) {
        snprintf(result_buf, sizeof(result_buf), "Invalid input\n");
    } else {
        switch (op) {
        case '+':
            res = a + b;
            break;
        case '-':
            res = a - b;
            break;
        case '*':
            res = a * b;
            break;
        case '/':
            if (b == 0) {
                snprintf(result_buf, sizeof(result_buf), "Division by zero\n");
                goto wake;
            }
            res = a / b;
            break;
        default:
            snprintf(result_buf, sizeof(result_buf), "Unknown operator\n");
            goto wake;
        }
        snprintf(result_buf, sizeof(result_buf), "Result: %d\n", res);
    }

wake:
    flag = 1;
    wake_up_interruptible(&wq);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};

static int __init calcwait_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev, 1);

    my_class = class_create(DEVICE_NAME);
    device_create(my_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "calcwait: Module loaded\n");
    return 0;
}

static void __exit calcwait_exit(void)
{
    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "calcwait: Module unloaded\n");
}

module_init(calcwait_init);
module_exit(calcwait_exit);

