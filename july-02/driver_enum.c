#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "simple"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mamta (Modified by ChatGPT)");
MODULE_DESCRIPTION("Character device calculator driver using enum");
MODULE_VERSION("1.0");

// Define enum for operations
enum operation {
    OP_ADD = 1,
    OP_SUB = 2,
    OP_MUL = 3,
    OP_DIV = 4
};

// Shared structure for communication
struct calc_data {
    int op1;
    int op2;
    enum operation choice;
    int result;
};

static int major;
static struct calc_data kernel_calc;

// Device open
static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_dev: Device opened\n");
    return 0;
}

// Device release
static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_dev: Device closed\n");
    return 0;
}

// Device read: return result to user space
static ssize_t dev_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    if (*ppos != 0)
        return 0;

    if (count != sizeof(struct calc_data))
        return -EINVAL;

    if (copy_to_user(user_buf, &kernel_calc, sizeof(kernel_calc)))
        return -EFAULT;

    *ppos += sizeof(kernel_calc);
    printk(KERN_INFO "simple_char_dev: Sent result: %d\n", kernel_calc.result);
    return sizeof(kernel_calc);
}

// Device write: receive operands and operation
static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    struct calc_data input;

    if (count != sizeof(struct calc_data)) {
        printk(KERN_WARNING "simple_char_dev: Invalid write size\n");
        return -EINVAL;
    }

    if (copy_from_user(&input, user_buf, sizeof(input)))
        return -EFAULT;

    switch (input.choice) {
        case OP_ADD:
            input.result = input.op1 + input.op2;
            break;
        case OP_SUB:
            input.result = input.op1 - input.op2;
            break;
        case OP_MUL:
            input.result = input.op1 * input.op2;
            break;
        case OP_DIV:
            if (input.op2 != 0)
                input.result = input.op1 / input.op2;
            else
                input.result = 0;  // Avoid division by zero
            break;
        default:
            printk(KERN_WARNING "simple_char_dev: Invalid operation: %d\n", input.choice);
            input.result = 0;
    }

    kernel_calc = input;

    printk(KERN_INFO "simple_char_dev: Performed op=%d on %d and %d => result=%d\n",
           input.choice, input.op1, input.op2, input.result);

    return sizeof(input);
}

// File operations struct
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

// Module init function
static int __init simple_char_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "simple_char_dev: Failed to register character device\n");
        return major;
    }

    printk(KERN_INFO "simple_char_dev: Registered with major number %d\n", major);
    printk(KERN_INFO "Create device node using: sudo mknod /dev/%s c %d 0\n", DEVICE_NAME, major);
    return 0;
}

// Module exit function
static void __exit simple_char_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "simple_char_dev: Unregistered device with major number %d\n", major);
}

// Register init and exit functions
module_init(simple_char_init);
module_exit(simple_char_exit);

