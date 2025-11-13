// led_gpio_driver.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/timer.h>
#include <linux/delay.h>

static struct gpio_desc *led_gpio;
static struct timer_list blink_timer;
static bool led_on = false;

static void blink_function(struct timer_list *t) {
    led_on = !led_on;
    gpiod_set_value(led_gpio, led_on);
    mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500)); // 500ms toggle
}

static int led_probe(struct platform_device *pdev) {
    int ret;

    led_gpio = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(led_gpio)) {
        dev_err(&pdev->dev, "Failed to get LED GPIO\n");
        return PTR_ERR(led_gpio);
    }

    timer_setup(&blink_timer, blink_function, 0);
    mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500));
    dev_info(&pdev->dev, "LED blink driver probed\n");

    return 0;
}

static int led_remove(struct platform_device *pdev) {
    del_timer_sync(&blink_timer);
    gpiod_set_value(led_gpio, 0);
    dev_info(&pdev->dev, "LED blink driver removed\n");
    return 0;
}

static const struct of_device_id led_dt_ids[] = {
    { .compatible = "blinky,gpio-led" },
    { }
};
MODULE_DEVICE_TABLE(of, led_dt_ids);

static struct platform_driver led_driver = {
    .probe = led_probe,
    .remove = led_remove,
    .driver = {
        .name = "gpio_led_blink",
        .of_match_table = led_dt_ids,
    },
};
module_platform_driver(led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Simple GPIO LED Blinker Platform Driver");

