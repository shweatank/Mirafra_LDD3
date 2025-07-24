#include <linux/module.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/kthread.h>

static struct gpio_desc *led_gpio;
static struct task_struct *blink_thread;

// Blinking function run in kernel thread
static int led_blink_fn(void *data)
{
    while (!kthread_should_stop()) {
        gpiod_set_value(led_gpio, 1);
        msleep(500);
        gpiod_set_value(led_gpio, 0);
        msleep(500);
    }
    return 0;
}

// Called when platform device is registered
static int led_blink_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;

    // Request the GPIO from device tree
    led_gpio = gpiod_get(dev, NULL, GPIOD_OUT_LOW);
    if (IS_ERR(led_gpio)) {
        dev_err(dev, "Failed to get LED GPIO\n");
        return PTR_ERR(led_gpio);
    }

    // Start kernel thread to blink LED
    blink_thread = kthread_run(led_blink_fn, NULL, "led_blink_thread");
    if (IS_ERR(blink_thread)) {
        dev_err(dev, "Failed to create blink thread\n");
        gpiod_put(led_gpio);
        return PTR_ERR(blink_thread);
    }

    dev_info(dev, "LED blink driver probed\n");
    return 0;
}

// Called when driver is removed or module is unloaded
static void led_blink_remove(struct platform_device *pdev)
{
    if (blink_thread)
        kthread_stop(blink_thread);
    gpiod_set_value(led_gpio, 0);
    gpiod_put(led_gpio);
}

static const struct of_device_id led_blink_dt_ids[] = {
    { .compatible = "rpi,gpio-led-blink" },
    { }
};
MODULE_DEVICE_TABLE(of, led_blink_dt_ids);

static struct platform_driver led_blink_driver = {
    .probe  = led_blink_probe,
    .remove = led_blink_remove,   // 🔄 Now matches correct signature (void)
    .driver = {
        .name = "gpio-led-blink",
        .of_match_table = led_blink_dt_ids,
    },
};

module_platform_driver(led_blink_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Custom LED Blink Driver for Raspberry Pi");

