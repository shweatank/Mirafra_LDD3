#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "enum_char_dev"
#define BUF_LEN 128

static int major;
static char device_buffer[BUF_LEN];
static int current_mode = 0; 

// Reverse string
static void reverse_string(char *str) 
{
    int len = strlen(str);
    int i;
    char temp;
    for (i = 0; i < len / 2; ++i)
    {
        temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

//  Convert string to uppercase
static void to_uppercase(char *str) 
{
    int i;
    for (i = 0; str[i]; ++i) 
    {
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] -= 32;
    }
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
    return count;
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    // If user writes an integer size of enum, interpret as command
    if (count == sizeof(int)) {
        int temp;
        if (copy_from_user(&temp, user_buf, sizeof(int)))
            return -EFAULT;

        current_mode = temp;
        printk(KERN_INFO "enum_char_dev: Mode set to %d\n", current_mode);
        return sizeof(int);
    }

    // If user writes string, process according to current mode
    if (count >= BUF_LEN)
        count = BUF_LEN - 1;

    if (copy_from_user(device_buffer, user_buf, count))
        return -EFAULT;

    device_buffer[count] = '\0';

    // Process based on mode
    switch (current_mode) {
        case 1:
            reverse_string(device_buffer);
            break;
        case 2:
            to_uppercase(device_buffer);
            break;
        default:
            break;
    }

    return count;
}

static struct file_operations fops = {
    .read = dev_read,
    .write = dev_write,
};

static int __init enum_char_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register device\n");
        return major;
    }
    printk(KERN_INFO "enum_char_dev: registered with major %d\n", major);
    return 0;
}

static void __exit enum_char_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "enum_char_dev: unregistered\n");
}

module_init(enum_char_init);
module_exit(enum_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kusuma");
MODULE_DESCRIPTION("Char driver using enum via write()");

