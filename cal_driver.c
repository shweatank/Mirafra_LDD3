#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "cal_driver"

static int major;
static int result = 0;

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    char kbuf[100];
    int num1, num2;
    char op[10];

    if (len > 99) 
	    len = 99;
    if (copy_from_user(kbuf, buf, len)) 
	    return -EFAULT;
    kbuf[len] = '\0';

   
    if (sscanf(kbuf, "%d %d %s", &num1, &num2, op) != 3) {
        printk(KERN_INFO "cal_dev: Invalid input format\n");
        return -EINVAL;
    }

    if (strcmp(op, "add") == 0) {
        result = num1 + num2;
    } else if (strcmp(op, "sub") == 0) {
        result = num1 - num2;
    } else if (strcmp(op, "mul") == 0) {
        result = num1 * num2;
    } else {
        printk(KERN_INFO "cal_dev: Unsupported operation '%s'\n", op);
        return -EINVAL;
    }

    printk(KERN_INFO "cal_dev: %d %s %d = %d\n", num1, op, num2, result);
    return len;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    char outbuf[20];
    int n = snprintf(outbuf, sizeof(outbuf), "%d", result);
    if (copy_to_user(buf, outbuf, n)) return -EFAULT;
    return n;
}

static int dev_open(struct inode *i, struct file *f) { return 0; }
static int dev_release(struct inode *i, struct file *f) { return 0; }

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .write = dev_write,
    .read = dev_read,
};

static int __init cal_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_INFO "cal_dev: loaded with major number %d\n", major);
    return 0;
}

static void __exit cal_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "cal_dev: unloaded\n");
}

module_init(cal_init);
module_exit(cal_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Afreen");
MODULE_DESCRIPTION("Simple calculator driver (add, sub, mul)");

