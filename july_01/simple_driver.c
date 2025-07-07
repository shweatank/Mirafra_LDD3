#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "simple_char_dev"
#define BUF_LEN 128

static int major;
static char device_buffer[BUF_LEN];

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_dev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_dev: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    int bytes_read = strlen(device_buffer);
    if (*ppos >= bytes_read)
        return 0;

    if (count > bytes_read - *ppos)
        count = bytes_read - *ppos;

    if (copy_to_user(user_buf, device_buffer + *ppos, count))
        return -EFAULT;

    *ppos += count;
    printk(KERN_INFO "simple_char_dev: Read %zu bytes\n", count);
    return count;
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    if (count>BUF_LEN - 1)
        count=BUF_LEN - 1;

    if (copy_from_user(device_buffer, user_buf, count))
        return -EFAULT;

    device_buffer[count] = '\0';
    printk(KERN_INFO "simple_char_dev: Written %zu bytes\n", count);
    return count;
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
    if (major < 0)
    {
        printk(KERN_ALERT "simple_char_dev: failed to register character device\n");
        return major;
    }
    printk(KERN_INFO "simple_char_dev: registered with major number %d\n", major);
    return 0;
}

static void __exit simple_char_exit(void)
{
    unregister_chrdev(major,DEVICE_NAME);
    printk(KERN_INFO "simple_char_dev:unregistered device\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("Minimal character device driver");
