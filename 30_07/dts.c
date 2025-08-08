// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>

#define DRIVER_NAME "led_blink_of"

static struct gpio_desc *led_gpio;

static int led_blink_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;

    led_gpio = devm_gpiod_get(dev, NULL, GPIOD_OUT_LOW);
    if (IS_ERR(led_gpio)) {
        dev_err(dev, "Failed to get LED GPIO from device tree\n");
        return PTR_ERR(led_gpio);
    }

    dev_info(dev, "LED GPIO acquired. Blinking...\n");

    // Simple blink loop for demo
    for (int i = 0; i < 5; i++) {
        gpiod_set_value(led_gpio, 1);
        msleep(200);
        gpiod_set_value(led_gpio, 0);
        msleep(200);
    }

    dev_info(dev, "LED blinking complete.\n");
    return 0;
}

static void led_blink_remove(struct platform_device *pdev)
{
    gpiod_set_value(led_gpio, 0);
  
}

static const struct of_device_id led_blink_dt_ids[] = {
    { .compatible = "rpi,led-blink", },
    {}
};
MODULE_DEVICE_TABLE(of, led_blink_dt_ids);

static struct platform_driver led_blink_driver = {
    .probe  = led_blink_probe,
    .remove = led_blink_remove,
    .driver = {
        .name           = DRIVER_NAME,
        .of_match_table = led_blink_dt_ids,
    },
};

module_platform_driver(led_blink_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba");
MODULE_DESCRIPTION("LED Blink Driver using of.h GPIO access on Raspberry Pi 4B");
