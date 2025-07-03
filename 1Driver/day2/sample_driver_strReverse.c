#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "reverser"
#define BUF_LEN 1024

static char message[BUF_LEN];
static int major;

static ssize_t dev_read(struct file *filp, char __user *buffer, size_t len, loff_t *offset) 
{
    size_t msg_len = strlen(message);
    if (*offset >= msg_len) return 0;
    if (copy_to_user(buffer, message, msg_len)) return -EFAULT;
    *offset = msg_len;
    return msg_len;
}

static ssize_t dev_write(struct file *filp, const char __user *buffer, size_t len, loff_t *offset) {
    if (len >= BUF_LEN) return -EINVAL;
    if (copy_from_user(message, buffer, len)) return -EFAULT;
    message[len] = '\0';

    // Reverse the string in-place
    size_t i = 0;
    size_t j = len - 1;
    char temp;

    while (i < j) {
        temp = message[i];
        message[i] = message[j];
        message[j] = temp;
        i++;
     }
    return len;
}

static struct file_operations fops = {
    .read = dev_read,
    .write = dev_write,
};

static int __init revdev_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Registering char device failed.\n");
        return major;
    }
    printk(KERN_INFO "Reverse device registered with major number %d\n", major);
    return 0;
}

static void __exit revdev_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Reverse device unregistered.\n");
}

module_init(revdev_init);
module_exit(revdev_exit);

MODULE_LICENSE("GPL");

