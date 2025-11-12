#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio/machine.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>

#define DRIVER_NAME "gpio_led_driver"

static struct gpiod_lookup_table led_gpio_table = {
	.dev_id = DRIVER_NAME,
	.table = {
		GPIO_LOOKUP("pinctrl-bcm2711", 21, NULL, GPIO_ACTIVE_HIGH),
		{ },
	},
};

static struct gpio_desc *led_gpio;

static int gpio_led_probe(struct platform_device *pdev)
{
	int i;

	pr_info("GPIO LED driver probe\n");

	led_gpio = gpiod_get(&pdev->dev, NULL, GPIOD_OUT_LOW);
	if (IS_ERR(led_gpio)) {
		pr_err("Failed to get GPIO\n");
		return PTR_ERR(led_gpio);
	}
	pr_info("LED GPIO acquired\n");

	/* Blink LED 5 times */
	for (i = 0; i < 5; i++) {
		gpiod_set_value(led_gpio, 1);
		msleep(500);
		gpiod_set_value(led_gpio, 0);
		msleep(500);
	}

	return 0;
}

static int gpio_led_remove(struct platform_device *pdev)
{
	pr_info("Removing GPIO LED driver\n");

	if (led_gpio)
		gpiod_put(led_gpio);
	return 0;
}

static struct platform_driver gpio_led_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.probe = gpio_led_probe,
	.remove = gpio_led_remove,
};

static struct platform_device *gpio_led_device;

static int __init gpio_led_init(void)
{
	gpiod_add_lookup_table(&led_gpio_table);
	gpio_led_device = platform_device_register_simple(DRIVER_NAME, -1 , NULL, 0);
	return platform_driver_register(&gpio_led_driver);
}

static void __exit gpio_led_exit(void)
{
	platform_driver_unregister(&gpio_led_driver);
	platform_device_unregister(gpio_led_device);
}

module_init(gpio_led_init);
module_exit(gpio_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priya");
MODULE_DESCRIPTION("Platform driver to blink LED using GPIO");

