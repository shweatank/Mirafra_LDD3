#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "calc_dev"
#define BUF_LEN 128

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priyalatha");
MODULE_DESCRIPTION("Calculator char device using struct");

static int major;

typedef struct {
    int operand1;
    int operand2;
    char operator;
    int result;
} Calc;

static Calc calc_data;

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "calc_dev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "calc_dev: Device closed\n");
    return 0;
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    if (count != sizeof(Calc))
        return -EINVAL;

    if (copy_from_user(&calc_data, user_buf, sizeof(Calc)))
        return -EFAULT;

    switch (calc_data.operator) {
        case '+': calc_data.result = calc_data.operand1 + calc_data.operand2; break;
        case '-': calc_data.result = calc_data.operand1 - calc_data.operand2; break;
        case '*': calc_data.result = calc_data.operand1 * calc_data.operand2; break;
        case '/':
            if (calc_data.operand2 == 0)
                calc_data.result = 0;  // or error code
            else
                calc_data.result = calc_data.operand1 / calc_data.operand2;
            break;
        default:
            printk(KERN_WARNING "calc_dev: Unknown operator '%c'\n", calc_data.operator);
            calc_data.result = 0;
            break;
    }

    printk(KERN_INFO "calc_dev: %d %c %d = %d\n",
           calc_data.operand1, calc_data.operator, calc_data.operand2, calc_data.result);

    return sizeof(Calc);
}

static ssize_t dev_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    if (count < sizeof(Calc))
        return -EINVAL;

    if (copy_to_user(user_buf, &calc_data, sizeof(Calc)))
        return -EFAULT;

    return sizeof(Calc);
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init calc_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "calc_dev: Failed to register device\n");
        return major;
    }
    printk(KERN_INFO "calc_dev: Registered with major number %d\n", major);
    return 0;
}

static void __exit calc_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "calc_dev: Unregistered\n");
}

module_init(calc_init);
module_exit(calc_exit);

