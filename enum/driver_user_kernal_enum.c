#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "calc_enum_1"
#define MAJOR_NUM   200
enum calc_op {
    OP_ADD = 0,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MAX
};

struct calc_data {
    int a;
    int b;
    enum calc_op op;
    int result;
};
static struct calc_data data;
static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "calc_enum_dev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "calc_enum_dev: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    if (*ppos != 0 || count < sizeof(struct calc_data))
        return 0;

    if (copy_to_user(user_buf, &data, sizeof(struct calc_data)))
        return -EFAULT;

    *ppos += sizeof(struct calc_data);
    printk(KERN_INFO "calc_enum_dev: Sent result %d to user\n", data.result);
    return sizeof(struct calc_data);
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    if (count < sizeof(struct calc_data))
        return -EINVAL;

    if (copy_from_user(&data, user_buf, sizeof(struct calc_data)))
        return -EFAULT;

    if (data.op < 0 || data.op >= OP_MAX)
        return -EINVAL;

    switch (data.op)
    {
        case OP_ADD:
            data.result = data.a + data.b;
            break;
        case OP_SUB:
            data.result = data.a - data.b;
            break;
        case OP_MUL:
            data.result = data.a * data.b;
            break;
        case OP_DIV:
            data.result = (data.b == 0) ? 0 : data.a / data.b;
            break;
        default:
            data.result = 0;
            break;
    }

    printk(KERN_INFO "calc_enum_dev: Calculated %d op=%d %d = %d\n", data.a, data.op, data.b, data.result);
    return sizeof(struct calc_data);
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};
static int __init calc_enum_init(void)
{
    int ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (ret < 0) {
        printk(KERN_ALERT "calc_enum_dev: Failed to register major number %d\n", MAJOR_NUM);
        return ret;
    }

    printk(KERN_INFO "calc_enum_dev: Registered with major number %d\n", MAJOR_NUM);
    return 0;
}

static void __exit calc_enum_exit(void)
{
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk(KERN_INFO "calc_enum_dev: Unregistered device\n");
}

module_init(calc_enum_init);
module_exit(calc_enum_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("Calculator char device using enum for operation");

