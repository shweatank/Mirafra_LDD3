#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char_dev"
#define BUF_LEN 100

static char kernel_buffer[BUF_LEN];
static int len = 0;

static int dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    if (*ppos >= len)
        return 0;

    if (copy_to_user(buf, kernel_buffer, len))
        return -EFAULT;

    *ppos += len;
    return len;
}

static void reverse_string(char *str, int n) {
    int i;
    for (i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - 1 - i];
        str[n - 1 - i] = temp;
    }
}

static ssize_t dev_write(struct file *file,
                         const char __user *buf,
                         size_t count,
                         loff_t *ppos)
{
    /* sanity‑check length */
    if (count > BUF_LEN)
        return -EINVAL;

    /* copy user data into kernel buffer */
    if (copy_from_user(kernel_buffer, buf, count))
        return -EFAULT;

    /* store the real length */
    len = count;

    /* -------- inline string‑reverse -------- */
    {
        size_t i;
        for (i = 0; i < len / 2; i++) {
            char tmp                 = kernel_buffer[i];
            kernel_buffer[i]         = kernel_buffer[len - 1 - i];
            kernel_buffer[len - 1 - i] = tmp;
        }
    }
    /* --------------------------------------- */

    printk(KERN_INFO "Reversed string stored: %.*s\n", (int)len, kernel_buffer);
    return count;          /* report number of bytes written */
}


static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int major;

static int __init simple_char_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Registering char device failed\n");
        return major;
    }
    printk(KERN_INFO "Device registered with major number %d\n", major);
    return 0;
}

static void __exit simple_char_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pritesh");
MODULE_DESCRIPTION("Char device that reverses string in kernel space");

