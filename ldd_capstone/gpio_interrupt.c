#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define DEVICE_NAME "audio_trigger"
#define GPIO_IN 529  // BCM GPIO 17 + 512

static int irq_number;
static int major;
static struct cdev audio_cdev;
static dev_t dev_num;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;

// Interrupt Service Routine
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "[AudioTrigger] GPIO Interrupt Occurred!\n");
    flag = 1;
    wake_up_interruptible(&wq);
    return IRQ_HANDLED;
}

// File operations
static ssize_t audio_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    printk(KERN_INFO "[AudioTrigger] Waiting for interrupt...\n");
    wait_event_interruptible(wq, flag != 0);
    flag = 0;

    char msg[] = "INTERRUPT\n";
    if (copy_to_user(buf, msg, sizeof(msg)))
        return -EFAULT;

    return sizeof(msg);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = audio_read,
};

static int __init audio_trigger_init(void)
{
    int ret;

    // Allocate character device region
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "[AudioTrigger] Failed to allocate char device region\n");
        return ret;
    }

    major = MAJOR(dev_num);
    cdev_init(&audio_cdev, &fops);
    audio_cdev.owner = THIS_MODULE;

    ret = cdev_add(&audio_cdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "[AudioTrigger] Failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    // Request GPIO
    if (!gpio_is_valid(GPIO_IN)) {
        printk(KERN_ERR "[AudioTrigger] Invalid GPIO\n");
        cdev_del(&audio_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -ENODEV;
    }

    gpio_request(GPIO_IN, "gpio_interrupt");
    gpio_direction_input(GPIO_IN);
    irq_number = gpio_to_irq(GPIO_IN);

    ret = request_irq(irq_number, gpio_irq_handler,
                      IRQF_TRIGGER_FALLING, "gpio_irq", NULL);
    if (ret) {
        printk(KERN_ERR "[AudioTrigger] IRQ request failed\n");
        gpio_free(GPIO_IN);
        cdev_del(&audio_cdev);
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    printk(KERN_INFO "[AudioTrigger] Module loaded: /dev/%s (major %d)\n", DEVICE_NAME, major);
    return 0;
}

static void __exit audio_trigger_exit(void)
{
    free_irq(irq_number, NULL);
    gpio_free(GPIO_IN);
    cdev_del(&audio_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "[AudioTrigger] Module unloaded\n");
}

module_init(audio_trigger_init);
module_exit(audio_trigger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sanaboina Pavan");
MODULE_DESCRIPTION("GPIO Interrupt-based Trigger Driver");

