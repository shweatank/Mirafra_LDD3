#include <linux/module.h>          // For all modules
#include <linux/init.h>            // For __init and __exit macros
#include <linux/fs.h>              // For file_operations
#include <linux/uaccess.h>         // For copy_to_user
#include <linux/gpio.h>            // For GPIO handling
#include <linux/interrupt.h>       // For IRQ handling
#include <linux/cdev.h>            // For cdev structure
#include <linux/wait.h>            // For wait queues
#include <linux/sched.h>           // For TASK_INTERRUPTIBLE

#define DEVICE_NAME "audio_trigger" // Device name in /dev
#define GPIO_IN 529                 // GPIO pin number (BCM 17 + 512 offset for RPi 4 GPIO)

static int irq_number;             // IRQ number for the GPIO
static int major;                  // Major number assigned to device
static struct cdev audio_cdev;     // Character device structure
static dev_t dev_num;              // Device number (major+minor)
static struct class *audio_class;  // Sysfs class for device
static struct device *audio_device;// Actual device created in /dev

// Wait queue and flag for interrupt signaling
static DECLARE_WAIT_QUEUE_HEAD(wq); 
static int flag = 0;

// Interrupt handler for GPIO pin
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "[AudioTrigger] GPIO Interrupt Occurred!\n");
    flag = 1;                                // Set flag
    wake_up_interruptible(&wq);              // Wake up user-space read
    return IRQ_HANDLED;
}

// File read operation
static ssize_t audio_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    printk(KERN_INFO "[AudioTrigger] Waiting for interrupt...\n");
    wait_event_interruptible(wq, flag != 0); // Block until flag is set
    flag = 0;                                // Reset for next interrupt

    char msg[] = "INTERRUPT\n";              // Message to send to user space
    if (copy_to_user(buf, msg, sizeof(msg)))
        return -EFAULT;

    return sizeof(msg);
}

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = audio_read,
};

// Initialization function
static int __init audio_trigger_init(void)
{
    int ret;

    // Allocate device number
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "[AudioTrigger] Failed to allocate char device region\n");
        return ret;
    }

    major = MAJOR(dev_num);                 // Extract major number
    cdev_init(&audio_cdev, &fops);          // Init char device
    audio_cdev.owner = THIS_MODULE;

    // Add char device to system
    ret = cdev_add(&audio_cdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "[AudioTrigger] Failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    // Create class
    audio_class = class_create("audio_class");
    if (IS_ERR(audio_class)) {
        printk(KERN_ERR "[AudioTrigger] Failed to create class\n");
        cdev_del(&audio_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(audio_class);
    }

    // Create device node in /dev
    audio_device = device_create(audio_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(audio_device)) {
        printk(KERN_ERR "[AudioTrigger] Failed to create device\n");
        class_destroy(audio_class);
        cdev_del(&audio_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(audio_device);
    }

    // Validate and configure GPIO
    if (!gpio_is_valid(GPIO_IN)) {
        printk(KERN_ERR "[AudioTrigger] Invalid GPIO\n");
        cdev_del(&audio_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -ENODEV;
    }

    gpio_request(GPIO_IN, "gpio_interrupt");
    gpio_direction_input(GPIO_IN);
    irq_number = gpio_to_irq(GPIO_IN);       // Map GPIO to IRQ number

    // Request IRQ
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

//Cleanup function
static void __exit audio_trigger_exit(void)
{
    free_irq(irq_number, NULL);              // Release IRQ
    gpio_free(GPIO_IN);                      // Free GPIO pin
    device_destroy(audio_class, dev_num);    // Remove device node
    class_destroy(audio_class);              // Destroy class
    cdev_del(&audio_cdev);                   // Delete char device
    unregister_chrdev_region(dev_num, 1);    // Free dev number
    printk(KERN_INFO "[AudioTrigger] Module unloaded\n");
}

module_init(audio_trigger_init);
module_exit(audio_trigger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sanaboina Pavan");
MODULE_DESCRIPTION("GPIO Interrupt-based Trigger Driver");

