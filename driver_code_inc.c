#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "inc_dev"

static int major;
static int value = 0;

// When user writes an integer to device
static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    char kbuf[10];

    if (len > 19) 
	    len = 19;
    if (copy_from_user(kbuf, buf, len)) 
	    return -EFAULT;

    kbuf[len] = '\0';                  // Null-terminate string
    kstrtoint(kbuf, 10, &value);       // Convert to int
    value++;                           // Increment the number

    return len;
}

// When user reads from device
static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    char kbuf[10];
    int n = snprintf(kbuf, sizeof(kbuf), "%d", value);

    if (copy_to_user(buf, kbuf, n)) 
	    return -EFAULT;
    return n;
}

static int dev_open(struct inode *inode, struct file *file) {
       	return 0; 
}
static int dev_release(struct inode *inode, struct file *file) {
       	return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .write = dev_write,
    .read = dev_read,
};


static int __init inc_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_INFO "inc_dev: loaded with major number %d\n", major);
    return 0;
}


static void __exit inc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "inc_dev: unloaded\n");
}

module_init(inc_init);
module_exit(inc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Afreen");
MODULE_DESCRIPTION("Simple driver to increment integer from user space");

