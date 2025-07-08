#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define DEVICE_NAME "mywait"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Waitqueue Calculator Example");

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;

struct st {
	int a;
	int b;
	char ch;
	int result;
} st;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;  // Condition for waking up

// --- Calculator logic ---
void calculator(struct st *data) {
	switch (data->ch) {
		case '+': data->result = data->a + data->b; break;
		case '-': data->result = data->a - data->b; break;
		default: data->result = 0; break;
	}
}

// --- Read blocks until flag is set ---
static ssize_t my_read(struct file *filp, char __user *buf,
                       size_t len, loff_t *off)
{
    printk(KERN_INFO "mywait: Reader going to sleep...\n");
    wait_event_interruptible(wq, flag != 0);

    printk(KERN_INFO "mywait: Reader woke up, flag = %d\n", flag);
    flag = 0;

    if (copy_to_user(buf, &st, sizeof(st)))
        return -EFAULT;

    return sizeof(st);
}

// --- Write sets flag and wakes up reader ---
static ssize_t my_write(struct file *filp, const char __user *buf,
                        size_t len, loff_t *off)
{
    if (copy_from_user(&st, buf, sizeof(st)))
        return -EFAULT;

    printk(KERN_INFO "mywait: Writer received: %d %c %d\n", st.a, st.ch, st.b);
    calculator(&st);
    flag = 1;
    wake_up_interruptible(&wq);
    return sizeof(st);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};

static int __init waitq_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev, 1);

    my_class = class_create(DEVICE_NAME); // OR use (THIS_MODULE, DEVICE_NAME)
    device_create(my_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "mywait: Module loaded\n");
    return 0;
}

static void __exit waitq_exit(void)
{
    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "mywait: Module unloaded\n");
}

module_init(waitq_init);
module_exit(waitq_exit);

