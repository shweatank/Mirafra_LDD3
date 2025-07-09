#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/init.h>
#include <linux/module.h>

#define DEVICE_NAME "waitq_calc"
#define length 64

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;

// Calculator structure
struct st {
    int op1;
    int op2;
    char ch;
    int res;
} ST;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;
static char result_str[64];  // result buffer

// Read operation
static ssize_t my_read(struct file *filp, char __user *ubuf,
                       size_t len, loff_t *off)
{
    printk(KERN_INFO "calc: Reader sleeping... (flag=%d)\n", flag);

    wait_event_interruptible(wq, flag != 0);

    printk(KERN_INFO "calc: Reader woke up. flag = %d\n", flag);

    if (copy_to_user(ubuf, result_str, strlen(result_str)))
        return -EFAULT;

    flag = 0;  // reset for next read
    return strlen(result_str);
}

// Write operation
static ssize_t my_write(struct file *filp, const char __user *buf,
                        size_t len, loff_t *off)
{
    char kbuf[length];

    if (len >= length)
        return -EINVAL;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    if (sscanf(kbuf, "%d %d %c", &ST.op1, &ST.op2, &ST.ch) != 3) 
    {
        printk(KERN_ERR "calc: Invalid input format\n");
        snprintf(result_str, sizeof(result_str), "Error: Invalid input\n");
        goto wakeup;
    }

    switch (ST.ch) {
        case '+':
            ST.res = ST.op1 + ST.op2;
            break;
        case '-':
            ST.res = ST.op1 - ST.op2;
            break;
        case '*':
            ST.res = ST.op1 * ST.op2;
            break;
        case '/':
            if (ST.op2 == 0) {
                printk(KERN_ERR "calc: Division by zero error\n");
                snprintf(result_str, sizeof(result_str), "Error: Division by zero\n");
                goto wakeup;
            }
            ST.res = ST.op1 / ST.op2;
            break;
        default:
            printk(KERN_ERR "calc: Invalid operator\n");
            snprintf(result_str, sizeof(result_str), "Error: Invalid operator\n");
            goto wakeup;
    }

    snprintf(result_str, sizeof(result_str), "Result: %d\n", ST.res);

wakeup:
    flag = 1;
    wake_up_interruptible(&wq);
    return len;
}

// File operations
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = my_read,
    .write = my_write,
};

// Module init
static int __init waitq_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "calc: Failed to allocate device number\n");
        return ret;
    }

    cdev_init(&my_cdev, &fops);
    ret = cdev_add(&my_cdev, dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev, 1);
        printk(KERN_ERR "calc: Failed to add cdev\n");
        return ret;
    }

    my_class = class_create(DEVICE_NAME);
    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        printk(KERN_ERR "calc: Failed to create class\n");
        return PTR_ERR(my_class);
    }

    device_create(my_class, NULL, dev, NULL, DEVICE_NAME);
    printk(KERN_INFO "calc: Device /dev/%s created\n", DEVICE_NAME);

    return 0;
}

// Module exit
static void __exit waitq_exit(void)
{
    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "calc: Device /dev/%s removed\n", DEVICE_NAME);
}

module_init(waitq_init);
module_exit(waitq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Kernel-space Calculator using Wait Queue");

