#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "int_increment"
#define BUF_LEN 128

static int major;
static int kernel_value = 0;

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_dev: Device Opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_dev: Device Closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    if (count < sizeof(int)) return -EINVAL;

    if (copy_to_user(user_buf, &kernel_value, sizeof(int)))
        return -EFAULT;

    printk(KERN_INFO "simple_char_dev: Sent value %d to user\n", kernel_value);
    return sizeof(int);
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    int temp;

    if (count < sizeof(int)) return -EINVAL;

    if (copy_from_user(&temp, user_buf, sizeof(int)))
        return -EFAULT;

    kernel_value = temp + 1;
    printk(KERN_INFO "simple_char_dev: Received %d, incremented to %d\n", temp, kernel_value);

    return sizeof(int);
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init simple_char_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "simple_char_dev: Registration failed\n");
        return major;
    }

    printk(KERN_INFO "simple_char_dev: Registered with major number %d\n", major);
    return 0;
}

static void __exit simple_char_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "simple_char_dev: Unregistered device\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priyalatha");
MODULE_DESCRIPTION("Character device for binary integer transfer");

