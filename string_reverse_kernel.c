#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "rev_dev"
#define BUF_LEN 100

static char kernel_buffer[BUF_LEN];
static int major;

// Helper function to reverse a string
void reverse(char *str, int len) {
    int i;
    char temp;
    for (i = 0; i < len / 2; i++) {
        temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

static ssize_t dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
    if (len > BUF_LEN - 1)
        len = BUF_LEN - 1;

    copy_from_user(kernel_buffer, buf, len);
    kernel_buffer[len] = '\0';

    reverse(kernel_buffer, strlen(kernel_buffer));
    return len;
}

static ssize_t dev_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    int data_len = strlen(kernel_buffer);
    if (copy_to_user(buf, kernel_buffer, data_len))
        return -EFAULT;
    return data_len;
}

static int dev_open(struct inode *i, struct file *f) { return 0; }
static int dev_close(struct inode *i, struct file *f) { return 0; }

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_close,
    .write = dev_write,
    .read = dev_read
};

static int __init rev_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_INFO "reverse_driver: loaded with major %d\n", major);
    return 0;
}

static void __exit rev_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "reverse_driver: unloaded\n");
}

module_init(rev_init);
module_exit(rev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Afreen");
MODULE_DESCRIPTION("Char driver to reverse string");

