#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "calculator"
#define BUF_LEN 1024

static char message[BUF_LEN];
static int major;

static ssize_t dev_read(struct file *filp, char __user *buffer, size_t len, loff_t *offset) 
{
    int msg_len = strlen(message);
    if (*offset >= msg_len) return 0;
    if (copy_to_user(buffer, message, msg_len)) return -EFAULT;
    *offset = msg_len;
    return msg_len;
}

static ssize_t dev_write(struct file *filp, const char __user *buffer, size_t len, loff_t *offset) {

    int op1,op2,res;
    char operator;
    if (len >= BUF_LEN) return -EINVAL;
    if (copy_from_user(message, buffer, len)) return -EFAULT;
    message[len] = '\0';

    if (sscanf(message, "%d %d %c", &op1, &op2, &operator) != 3) 
    {
        snprintf(message, BUF_LEN, "Invalid input\n");
        return -EINVAL;
    }
    switch(operator)
    {
	    case '+': res = op1+op2; break;
	    case '-': res = op1 - op2; break;
            case '*': res = op1 * op2; break;
            case '/': 
            if (op2 == 0) {
                snprintf(message,BUF_LEN, "Error: Division by zero\n");
                return -EINVAL;
            }
            res = op1 / op2; 
            break;
            default:
            snprintf(message, BUF_LEN, "Invalid operator\n");
            return -EINVAL;
    }

     snprintf(message, BUF_LEN, "%d\n", res);  // Store result to send via read()
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

