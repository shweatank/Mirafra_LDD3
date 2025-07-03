#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "calc"
static int major;

struct calc {
    int operand1;
    int operand2;
    char operator;
    int result;
};

static struct calc calc;  // <- structure variable renamed here

static int dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "calc: device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "calc: device closed\n");
    return 0;
}

static ssize_t dev_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    if (copy_from_user(&calc, buffer, sizeof(struct calc)))
        return -EFAULT;

    switch (calc.operator) {
        case '+':
            calc.result = calc.operand1 + calc.operand2;
            break;
        case '-':
            calc.result = calc.operand1 - calc.operand2;
            break;
        case '*':
            calc.result = calc.operand1 * calc.operand2;
            break;
        case '/':
            if (calc.operand2 != 0)
                calc.result = calc.operand1 / calc.operand2;
            else
                calc.result = 0;  // Prevent divide-by-zero
            break;
        default:
            calc.result = 0;  // Unknown operator
    }

    return sizeof(calc);
}

static ssize_t dev_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    if (copy_to_user(buffer, &calc, sizeof(struct calc)))
        return -EFAULT;
    return sizeof(calc);
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .write = dev_write,
    .read = dev_read,
};

static int __init calculator_driver_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "calc: registration failed\n");
        return major;
    }
    printk(KERN_INFO "calc: registered with major number %d\n", major);
    return 0;
}

static void __exit calculator_driver_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "calc: unregistered\n");
}

module_init(calculator_driver_init);
module_exit(calculator_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Calculator character device driver using struct calc");

