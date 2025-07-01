#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char_dev"
#define BUF_SIZE 128

static char device_buffer[BUF_SIZE];
static int major;

// open function
static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Simple_char_dev Device opened\n");
    return 0;
}

// release function
static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Simple_Char_Dev is Device closed\n");
    return 0;
}
// read function
static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t len, loff_t *offset) 
{
    int bytes_read = strlen(device_buffer);
    if (copy_to_user(user_buffer, device_buffer, bytes_read))
    {
        return -EFAULT;
    }
    printk(KERN_INFO "Simple_char_dev:Read %d bytes\n", bytes_read);
    return bytes_read;
}

// write function
static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *offset) 
{
    if(len>BUF_SIZE-1)
	    len=BUF_SIZE-1;
    if (copy_from_user(device_buffer, user_buffer,len)) 
    {
        return -EFAULT;
    }
    device_buffer[len]='\0';
    printk(KERN_INFO "simple_char_dev:Wrote %d bytes\n", len);
    return len;
}

// file operations
static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};
static int __init simple_char_init(void) 
{
    major = register_chrdev(0,DEVICE_NAME,&fops);
    if (major<0) {
        printk(KERN_ALERT "Failed to register devicer\n");
        return major;
    }
    printk(KERN_INFO "Simple char device registered with major number:%d\n",major);
    return 0;
}

static void __exit simple_char_exit(void)
{
    unregister_chrdev(major,DEVICE_NAME);
    printk(KERN_INFO "Simple char device is unregistered\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GH");
MODULE_DESCRIPTION("Simple Character Device Driver");

