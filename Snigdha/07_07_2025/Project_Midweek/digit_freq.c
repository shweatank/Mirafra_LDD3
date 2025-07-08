#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/keyboard.h>
#include <linux/slab.h>
#include <asm/io.h>

#define DEVICE_NAME "keyfreq"
#define KBD_IRQ 1
#define KBD_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snigdha");
MODULE_DESCRIPTION("Key Frequency Counter for digits 0-9 using Tasklet, Workqueue, Waitqueue");
MODULE_VERSION("1.0");

static int freq[10] = {0};
static int data_ready = 0;
static DECLARE_WAIT_QUEUE_HEAD(wq);

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;

static struct tasklet_struct my_tasklet;
static struct workqueue_struct *log_wq;
static DECLARE_DELAYED_WORK(log_work, NULL);


static void tasklet_func(struct tasklet_struct *t)
{
    unsigned char scancode = inb(KBD_DATA_PORT);
    int digit = -1;

    // Only consider key press (ignore release)
    if (!(scancode & 0x80)) 
    {
        switch (scancode) 
	{
            case 0x0B: digit = 0; break;
            case 0x02: digit = 1; break;
            case 0x03: digit = 2; break;
            case 0x04: digit = 3; break;
            case 0x05: digit = 4; break;
            case 0x06: digit = 5; break;
            case 0x07: digit = 6; break;
            case 0x08: digit = 7; break;
            case 0x09: digit = 8; break;
            case 0x0A: digit = 9; break;
        }

        if (digit >= 0) 
	{
            freq[digit]++;
            data_ready = 1;
            wake_up_interruptible(&wq);
        }
    }
}

static void log_work_func(struct work_struct *work)
{
    int i;
    printk(KERN_INFO "Digit Frequency Log:\n");
    for (i = 0; i < 10; i++)
        printk(KERN_INFO "Key %d: %d times\n", i, freq[i]);

    queue_delayed_work(log_wq, &log_work, msecs_to_jiffies(10000));
}


static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&my_tasklet);
    return IRQ_HANDLED;
}


static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    char *kbuf;
    int i, ret = 0;

    if (*off > 0)
        return 0;

    wait_event_interruptible(wq, data_ready);
    data_ready = 0;

    kbuf = kmalloc(256, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    ret += sprintf(kbuf, "Digit Frequencies:\n");
    for (i = 0; i < 10; i++)
        ret += sprintf(kbuf + ret, "Key %d: %d times\n", i, freq[i]);

    if (copy_to_user(buf, kbuf, ret)) 
    {
        kfree(kbuf);
        return -EFAULT;
    }

    kfree(kbuf);
    *off = ret;
    return ret;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    int i;
    for (i = 0; i < 10; i++)
        freq[i] = 0;
    data_ready = 1;
    wake_up_interruptible(&wq);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};


static int __init keyfreq_init(void)
{
    int ret;

    printk(KERN_INFO "KeyFreq: Initializing...\n");

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev, 1);
    my_class = class_create(DEVICE_NAME);
    device_create(my_class, NULL, dev, NULL, DEVICE_NAME);


    tasklet_setup(&my_tasklet, tasklet_func);
    ret = request_irq(KBD_IRQ, keyboard_irq_handler, IRQF_SHARED, "keyfreq_irq", (void *)&dev);
    if (ret) 
    {
        pr_err("KeyFreq: Failed to request IRQ\n");
        return ret;
    }

    log_wq = alloc_workqueue("keyfreq_log_wq", WQ_UNBOUND, 0);
    INIT_DELAYED_WORK(&log_work, log_work_func);
    queue_delayed_work(log_wq, &log_work, msecs_to_jiffies(10000));

    printk(KERN_INFO "KeyFreq: Module loaded\n");
    return 0;
}


static void __exit keyfreq_exit(void)
{
    cancel_delayed_work_sync(&log_work);
    destroy_workqueue(log_wq);
    
    tasklet_kill(&my_tasklet);
    free_irq(KBD_IRQ, (void *)&dev);
    
    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "KeyFreq: Module unloaded\n");
}

module_init(keyfreq_init);
module_exit(keyfreq_exit);

