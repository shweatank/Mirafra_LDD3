#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/kernel.h>

#define DEVICE_NAME "read_write_char_dev"
#define BUF_LEN 128

static int major;
static char device_buffer[BUF_LEN];

// Convert string to integer (simple implementation)
static int string_to_int(const char *str)
{
    int num = 0;
    int sign = 1;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str && *str >= '0' && *str <= '9') {
        num = num * 10 + (*str - '0');
        str++;
    }

    return num * sign;
}

// Convert integer to string
static void int_to_string(int num, char *str)
{
    int i = 0;
    int is_negative = 0;
    char temp[BUF_LEN];

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0) {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (is_negative)
        temp[i++] = '-';

    // Reverse temp into str
    int j = 0;
    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}

// File operations
static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "read_write_char_dev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "read_write_char_dev: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    int len = strlen(device_buffer);

    if (*ppos >= len)
        return 0;

    if (count > len - *ppos)
        count = len - *ppos;

    if (copy_to_user(user_buf, device_buffer + *ppos, count))
        return -EFAULT;

    *ppos += count;
    printk(KERN_INFO "read_write_char_dev: Sent incremented value to user\n");
    return count;
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    char temp_buf[BUF_LEN];
    int num;

    if (count >= BUF_LEN)
        count = BUF_LEN - 1;

    if (copy_from_user(temp_buf, user_buf, count))
        return -EFAULT;

    temp_buf[count] = '\0';

    // Convert to int, increment, and back to string
    num = string_to_int(temp_buf);
    printk(KERN_INFO "read_write_char_dev: Received %d from user\n", num);
    num += 1;
    int_to_string(num, device_buffer);

    printk(KERN_INFO "read_write_char_dev: Incremented to %d, sent back as \"%s\"\n", num, device_buffer);
    return count;
}

// File operations structure
static struct file_operations fops =
{
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

// Module init
static int __init simple_char_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register character device\n");
        return major;
    }

    printk(KERN_INFO "read_write_char_dev: Registered with major number %d\n", major);
    return 0;
}

// Module exit
static void __exit simple_char_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "read_write_char_dev: Unregistered device\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Character device driver that increments an integer from user space");

