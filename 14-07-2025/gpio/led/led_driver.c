#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>         // For legacy GPIO APIs
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/init.h>

#define DRIVER_NAME "led_gpio_driver"
#define DEVICE_NAME "led_gpio"
#define CLASS_NAME  "led"

#define GPIO_LED 17+512    // BCM GPIO 17

static int major;
static struct class*  led_class  = NULL;
static struct device* led_device = NULL;

static int led_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "LED: Device opened\n");
    return 0;
}

static int led_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "LED: Device closed\n");
    return 0;
}

static ssize_t led_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    char msg[2];

    if (copy_from_user(msg, buffer, len)) {
        return -EFAULT;
    }

    if (msg[0] == '1') {
        gpio_set_value(GPIO_LED, 1);
        printk(KERN_INFO "LED: ON\n");
    } else if (msg[0] == '0') {
        gpio_set_value(GPIO_LED, 0);
        printk(KERN_INFO "LED: OFF\n");
    }

    return len;
}

static struct file_operations fops = {
    .open = led_open,
    .write = led_write,
    .release = led_release,
};

static int __init led_init(void) {
    printk(KERN_INFO "LED: Initializing driver...\n");

    // Request the GPIO
    if (!gpio_is_valid(GPIO_LED)) {
        printk(KERN_ALERT "LED: Invalid GPIO %d\n", GPIO_LED);
        return -ENODEV;
    }

    gpio_request(GPIO_LED, "sysfs");
    gpio_direction_output(GPIO_LED, 0);
   // gpio_export(GPIO_LED, false);

    // Register char device
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "LED: Failed to register major number\n");
        return major;
    }

    // Register device class
    led_class = class_create(CLASS_NAME);
    if (IS_ERR(led_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "LED: Failed to register class\n");
        return PTR_ERR(led_class);
    }

    // Register device driver
    led_device = device_create(led_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(led_device)) {
        class_destroy(led_class);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "LED: Failed to create device\n");
        return PTR_ERR(led_device);
    }

    printk(KERN_INFO "LED: Driver loaded successfully. Use /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit led_exit(void) {
    gpio_set_value(GPIO_LED, 0);
 //   gpio_unexport(GPIO_LED);
    gpio_free(GPIO_LED);

    device_destroy(led_class, MKDEV(major, 0));
    class_unregister(led_class);
    class_destroy(led_class);
    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "LED: Driver unloaded\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba");
MODULE_DESCRIPTION("GPIO LED Blink Driver for Raspberry Pi 4B");
