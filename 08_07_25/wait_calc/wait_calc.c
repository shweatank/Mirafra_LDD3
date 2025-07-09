#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/version.h>  // for kernel version check
#include <linux/jiffies.h>  // for jiffies

#define DEVICE_NAME "mycal"
#define TIMEOUT_MS 5000  // 5 seconds

MODULE_LICENSE("GPL");

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;
#define MAX_BUF_LEN 128

static DECLARE_WAIT_QUEUE_HEAD(my_queue);
static int condition = 0;

static char kernel_buffer[MAX_BUF_LEN];
static int result = 0;
static size_t result_len = 0;

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    long ret;

    printk(KERN_INFO "mychar: read() waiting for condition OR signal OR timeout\n");

    ret = wait_event_interruptible_timeout(
        my_queue,
        condition != 0,
        msecs_to_jiffies(TIMEOUT_MS)
    );

    if (ret == 0) {
        printk(KERN_INFO "mychar: Timed out after %d ms\n", TIMEOUT_MS);
        return -ETIMEDOUT;
    } else if (ret < 0) {
        printk(KERN_INFO "mychar: Interrupted by signal during wait\n");
        return -EINTR;
    }
    result_len=strnlen(kernel_buffer, MAX_BUF_LEN);
      if (copy_to_user(buf, kernel_buffer, result_len))
        return -EFAULT;

    condition = 0;
   return result_len;
}
static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{

    printk(KERN_INFO "mychar: write() called, waking up waiters\n");
    char temp[20];
    int num1=0,num2=0;
    
     // Copy data from user to kernel
    if (copy_from_user(temp, buf, min(count, sizeof(temp) - 1)))
        return -EFAULT;

    // Null-terminate the string
    temp[min(count, sizeof(temp) - 1)] = '\0';

    // Extract two integers from input string
    if (sscanf(temp, "%d %d", &num1, &num2) != 2)
        return -EINVAL;

    // Perform the operation (add here)
    result = num1 + num2;

    // Prepare result string
    snprintf(kernel_buffer, sizeof(kernel_buffer), "%d\n", result);
    result_len = strnlen(kernel_buffer, MAX_BUF_LEN);

    condition = 1;
    wake_up_interruptible(&my_queue);
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};

static int __init my_init(void)

{
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev, 1);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)
    my_class = class_create(DEVICE_NAME);
#else
    my_class = class_create(THIS_MODULE, DEVICE_NAME);
#endif

  device_create(my_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "mychar: Module loaded with timeout support\n");
    return 0;
}

static void __exit my_exit(void)
{
    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "mychar: Module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);


