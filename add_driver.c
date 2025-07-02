#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "add_dev"

static int major;
static int result = 0;

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    char kbuf[100];
    int a, b, c;

    if (len > 99) 
	    len = 99;
    if (copy_from_user(kbuf, buf, len)) 
	    return -EFAULT;

    kbuf[len] = '\0';

    
    if (sscanf(kbuf, "%d %d %d", &a, &b, &c) != 3) {
        printk(KERN_INFO "add_dev: Invalid input format\n");
        return -EINVAL;
    }

    result = a + b + c;
    printk(KERN_INFO "add_dev: Received %d, %d, %d -> Sum = %d\n", a, b, c, result);
    return len;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    char outbuf[20];
    int n = snprintf(outbuf, sizeof(outbuf), "%d", result);

    if (copy_to_user(buf, outbuf, n)) 
	    return -EFAULT;
    return n;
}

static int dev_open(struct inode *i, struct file *f) { 
	return 0; 
}
static int dev_release(struct inode *i, struct file *f) { 
	return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .write = dev_write,
    .read = dev_read,
};

static int __init add_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_INFO "add_dev: loaded with major number %d\n", major);
    return 0;
}

static void __exit add_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "add_dev: unloaded\n");
}

module_init(add_init);
module_exit(add_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Afreen");
MODULE_DESCRIPTION("Char driver to add numbers from user");

