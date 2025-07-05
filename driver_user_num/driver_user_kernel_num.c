#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char_num1"
static int major;
static int result = 0; 

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_num: Device opened\n");
    return 0;
}
static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_num: Device closed\n");
    return 0;
}
static ssize_t dev_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    if (*ppos > 0 || count < sizeof(int))
        return 0;

    if (copy_to_user(user_buf, &result, sizeof(int)))
        return -EFAULT;

    *ppos += sizeof(int);
    printk(KERN_INFO "simple_char_num: Returned result = %d\n", result);
    return sizeof(int);
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    int input = 0;

    if (count < sizeof(int))
        return -EINVAL;

    if (copy_from_user(&input, user_buf, sizeof(int)))
        return -EFAULT;

    printk(KERN_INFO "simple_char_num: Raw bytes received = %02x %02x %02x %02x\n",
           ((unsigned char*)&input)[0], ((unsigned char*)&input)[1],
           ((unsigned char*)&input)[2], ((unsigned char*)&input)[3]);

    result = input + 1;
    printk(KERN_INFO "simple_char_num: Received %d, incremented to %d\n", input, result);
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
        printk(KERN_ALERT "simple_char_num: Failed to register device\n");
        return major;
    }
    printk(KERN_INFO "simple_char_num: Registered with major number %d\n", major);
    return 0;
}
static void __exit simple_char_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "simple_char_num: Device unregistered\n");
}
module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("Simple char device with binary int I/O and debug");

