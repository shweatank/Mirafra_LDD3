#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/ioctl.h>

#define DRIVER_NAME "led_gpio_driver"
#define DEVICE_NAME "led_gpio"

#define GPIO_LED (17 + 512)

#define LED_MAGIC 'L'
#define LED_ON    _IO(LED_MAGIC, 0)
#define LED_OFF   _IO(LED_MAGIC, 1)
static int major;
static long led_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case LED_ON:
            gpio_set_value(GPIO_LED, 1);
            printk(KERN_INFO "LED: Turned ON via ioctl\n");
            break;

        case LED_OFF:
            gpio_set_value(GPIO_LED, 0);
            printk(KERN_INFO "LED: Turned OFF via ioctl\n");
            break;

        default:
            printk(KERN_WARNING "LED: Invalid ioctl command\n");
            return -EINVAL;
    }
    return 0;
}
static int led_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "LED: Device opened\n");
    return 0;
}
static int led_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "LED: Device closed\n");
    return 0;
}
static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = led_open,
    .release        = led_release,
    .unlocked_ioctl = led_ioctl,
};
static int __init led_init(void) {
    int ret;
    printk(KERN_INFO "LED: Initializing driver...\n");
    if (!gpio_is_valid(GPIO_LED)) {
        printk(KERN_ALERT "LED: Invalid GPIO %d\n", GPIO_LED);
        return -ENODEV;
    }

    ret = gpio_request(GPIO_LED, "led_gpio");
    if (ret) {
        printk(KERN_ALERT "LED: Failed to request GPIO %d\n", GPIO_LED);
        return ret;
    }

    gpio_direction_output(GPIO_LED, 0);  // LED off initially

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "LED: Failed to register major number\n");
        gpio_free(GPIO_LED);
        return major;
    }

    printk(KERN_INFO "LED: Registered with major number %d\n", major);
    printk(KERN_INFO "LED: Create device node manually with:\n");
    printk(KERN_INFO "      mknod /dev/%s c %d 0\n", DEVICE_NAME, major);

    return 0;
}

static void __exit led_exit(void) {
    gpio_set_value(GPIO_LED, 0); 
    gpio_free(GPIO_LED);

    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "LED: Driver unloaded\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("team2");
MODULE_DESCRIPTION("LED GPIO Driver using ioctl, no class/device creation");

