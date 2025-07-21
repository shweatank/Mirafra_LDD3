#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/init.h>

#define DRIVER_NAME "led_gpio_driver"
#define DEVICE_NAME "led_gpio"
#define CLASS_NAME  "led"

#define GPIO_LED 529   // BCM GPIO 17

static int major;
static struct class*  led_class  = NULL;
static struct device* led_device = NULL;

static int led_open(struct inode *inodep, struct file *filep) {
    pr_info("LED: Device opened\n");
    return 0;
}

static int led_release(struct inode *inodep, struct file *filep) {
    pr_info("LED: Device closed\n");
    return 0;
}

static ssize_t led_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    char msg[2] = {0};

    if (len > 2)
        return -EINVAL;

    if (copy_from_user(msg, buffer, 1)) {
        pr_err("LED: Failed to receive data from user\n");
        return -EFAULT;
    }

    if (msg[0] == '1') {
        gpio_set_value(GPIO_LED, 1);
        pr_info("LED: ON\n");
    } else if (msg[0] == '0') {
        gpio_set_value(GPIO_LED, 0);
        pr_info("LED: OFF\n");
    } else {
        pr_warn("LED: Invalid write value. Use '1' or '0'\n");
        return -EINVAL;
    }

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .write = led_write,
    .release = led_release,
};

static int __init led_init(void) {
    int ret;

    pr_info("LED: Initializing driver...\n");

    if (!gpio_is_valid(GPIO_LED)) {
        pr_err("LED: Invalid GPIO %d\n", GPIO_LED);
        return -ENODEV;
    }

    ret = gpio_request(GPIO_LED, "LED_GPIO");
    if (ret) {
        pr_err("LED: GPIO request failed\n");
        return ret;
    }

    gpio_direction_output(GPIO_LED, 0);

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("LED: Failed to register char device\n");
        gpio_free(GPIO_LED);
        return major;
    }

    led_class = class_create(CLASS_NAME);  // Only 1 argument now
    if (IS_ERR(led_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        gpio_free(GPIO_LED);
        pr_err("LED: Failed to create class\n");
        return PTR_ERR(led_class);
    }

    led_device = device_create(led_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(led_device)) {
        class_destroy(led_class);
        unregister_chrdev(major, DEVICE_NAME);
        gpio_free(GPIO_LED);
        pr_err("LED: Failed to create device\n");
        return PTR_ERR(led_device);
    }

    pr_info("LED: Driver loaded successfully. Use /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit led_exit(void) {
    gpio_set_value(GPIO_LED, 0);
    gpio_free(GPIO_LED);

    device_destroy(led_class, MKDEV(major, 0));
    class_unregister(led_class);
    class_destroy(led_class);
    unregister_chrdev(major, DEVICE_NAME);

    pr_info("LED: Driver unloaded\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba");
MODULE_DESCRIPTION("GPIO LED Driver for Raspberry Pi - Kernel 6.12+");
MODULE_VERSION("1.1");

