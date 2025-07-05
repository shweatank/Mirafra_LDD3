#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#define DEVICE_NAME "calculator"
static int major;
struct calc_data {
    int a;
    int b;
    char op;
    int result;
};
static struct calc_data data;

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

static ssize_t dev_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    if (*ppos != 0 || count < sizeof(struct calc_data))
        return 0;

    if (copy_to_user(user_buf, &data, sizeof(struct calc_data)))
        return -EFAULT;

    *ppos += sizeof(struct calc_data);
    printk(KERN_INFO "calc_dev: Sent result %d to user\n", data.result);
    return sizeof(struct calc_data);
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    if (count < sizeof(struct calc_data))
        return -EINVAL;

    if (copy_from_user(&data, user_buf, sizeof(struct calc_data)))
        return -EFAULT;

    switch (data.op)
    {
        case '+': data.result = data.a + data.b; break;
        case '-': data.result = data.a - data.b; break;
        case '*': data.result = data.a * data.b; break;
        case '/': data.result = (data.b != 0) ? data.a / data.b : 0; break;
        default: data.result = 0; break;
    }

    printk(KERN_INFO "calc_dev: Calculated %d %c %d = %d\n", data.a, data.op, data.b, data.result);
    return sizeof(struct calc_data);
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
    if (major < 0)
    {
        printk(KERN_ALERT "calc_dev: failed to register device\n");
        return major;
    }
    printk(KERN_INFO "calc_dev: registered with major number %d\n", major);
    return 0;
}

static void __exit calc_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "calc_dev: unregistered device\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("Calculator char device using structure");

