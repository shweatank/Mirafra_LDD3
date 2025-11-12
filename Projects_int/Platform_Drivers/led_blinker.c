#include<linux/module.h>
#include<linux/platform_device.h>
#include<linux/of.h>
#include<linux/gpio/consumer.h>
#include<linux/timer.h>
#include<linux/of_gpio.h>
#include<linux/delay.h>

static struct gpio_desc *led_gpio;
static struct timer_list  blink_timer;
static bool led_on = false;

static void blink_timer_callback(struct timer_list *t)
{
	led_on= !led_on;
	gpiod_set_value(led_gpio, led_on);
	mod_timer(&blink_timer, jiffies + msecs_to_jiffies(1000));
}

static int led_probe(struct platform_device *pdev)
{
	pr_info("LED Blink: Probe called\n");

	led_gpio = gpiod_get(&pdev->dev,NULL, GPIOD_OUT_LOW);
	if(IS_ERR(led_gpio))
	{
		pr_err("Failed to get GPIO\n");
		return PTR_ERR(led_gpio);
	}

	timer_setup(&blink_timer, blink_timer_callback,0);
	mod_timer(&blink_timer, jiffies + msecs_to_jiffies(1000));

	return 0;
}

static int led_remove(struct platform_device *pdev)
{
	del_timer_sync(&blink_timer);
	gpiod_set_value(led_gpio,0);
	gpiod_put(led_gpio);
	pr_info("LED Blink Removed\n");
	return 0;
}

static const struct of_device_id led_dt_ids[]= {
	{ .compatible = "priya,led-blinker"},
	{ }
};

MODULE_DEVICE_TABLE(of, led_dt_ids);

static struct platform_driver led_driver = {
	.probe = led_probe,
	.remove = led_remove,
	.driver = {
		.name = "led_blinker",
		.of_match_table = led_dt_ids,
	},
};

module_platform_driver(led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priyalatha");
MODULE_DESCRIPTION("LED Blink using GPIO and Device Tree");

