#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/pwm.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#define DEVICE_NAME "virt_uart_led"
#define CLASS_NAME  "virt_uart"  // changed to avoid duplication

#define GPIO_LED 18               // GPIO18 (Pin 12)
#define PWM_LABEL "pwm-0:0"       // use exact PWM label seen in /sys/class/pwm
#define PWM_PERIOD_NS 1000000     // 1 ms = 1kHz

static int major;
static struct class *virt_class;
static struct device *virt_device;
static struct pwm_device *pwm;

static struct hrtimer blink_timer;
static bool led_on = false;
static bool blinking = false;

static enum hrtimer_restart blink_callback(struct hrtimer *timer)
{
    led_on = !led_on;
    gpio_set_value(GPIO_LED, led_on);
    hrtimer_forward_now(timer, ns_to_ktime(500000000));  // 500ms
    return blinking ? HRTIMER_RESTART : HRTIMER_NORESTART;
}

static ssize_t driver_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    char kbuf[64];
    long duty;

    if (len > sizeof(kbuf) - 1)
        return -EINVAL;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    if (strncmp(kbuf, "on", 2) == 0) {
        blinking = false;
        gpio_set_value(GPIO_LED, 1);
    } else if (strncmp(kbuf, "off", 3) == 0) {
        blinking = false;
        gpio_set_value(GPIO_LED, 0);
    } else if (strncmp(kbuf, "blink", 5) == 0) {
        blinking = true;
        hrtimer_start(&blink_timer, ns_to_ktime(500000000), HRTIMER_MODE_REL);
    } else if (strncmp(kbuf, "stop", 4) == 0) {
        blinking = false;
        hrtimer_cancel(&blink_timer);
        gpio_set_value(GPIO_LED, 0);
    } else if (sscanf(kbuf, "bright %ld", &duty) == 1) {
        if (duty >= 0 && duty <= 100) {
            int ret = pwm_config(pwm, (PWM_PERIOD_NS * duty) / 100, PWM_PERIOD_NS);
            if (ret < 0)
                pr_err("Failed to configure PWM duty cycle\n");
            else
                pwm_enable(pwm);
        }
    }

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = driver_write,
};

static int __init pwm_led_init(void)
{
    int ret;

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register char device\n");
        return major;
    }

    virt_class = class_create(CLASS_NAME);
    if (IS_ERR(virt_class)) {
        pr_err("Failed to create class\n");
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(virt_class);
    }

    virt_device = device_create(virt_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(virt_device)) {
        pr_err("Failed to create device\n");
        class_destroy(virt_class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(virt_device);
    }

    if (!gpio_is_valid(GPIO_LED)) {
        pr_err("Invalid GPIO\n");
        goto fail;
    }

    gpio_request(GPIO_LED, "LED");
    gpio_direction_output(GPIO_LED, 0);

    pwm = pwm_get(NULL, PWM_LABEL);
    if (IS_ERR(pwm)) {
        pr_err("Failed to request PWM %s\n", PWM_LABEL);
        goto fail;
    }

    ret = pwm_config(pwm, 0, PWM_PERIOD_NS);
    if (ret < 0) {
        pr_err("Failed to configure PWM\n");
        pwm_put(pwm);
        goto fail;
    }

    pwm_enable(pwm);

    hrtimer_init(&blink_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    blink_timer.function = blink_callback;

    pr_info("UART-GPIO-PWM-Timer LED Driver loaded\n");
    return 0;

fail:
    device_destroy(virt_class, MKDEV(major, 0));
    class_destroy(virt_class);
    unregister_chrdev(major, DEVICE_NAME);
    return -1;
}

static void __exit pwm_led_exit(void)
{
    blinking = false;
    hrtimer_cancel(&blink_timer);

    if (!IS_ERR_OR_NULL(pwm)) {
        pwm_disable(pwm);
        pwm_put(pwm);
    }

    gpio_set_value(GPIO_LED, 0);
    gpio_free(GPIO_LED);

    device_destroy(virt_class, MKDEV(major, 0));
    class_destroy(virt_class);
    unregister_chrdev(major, DEVICE_NAME);

    pr_info("Driver unloaded\n");
}

module_init(pwm_led_init);
module_exit(pwm_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha");
MODULE_DESCRIPTION("UART to GPIO/PWM/Timer LED Control on Raspberry Pi");

