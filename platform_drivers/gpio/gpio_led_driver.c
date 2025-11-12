#include<linux/module.h>
#include<linux/of.h>
#include<linux/platform_device.h>
#include<linux/gpio.h>
#include<linux/of_gpio.h>
#include<linux/delay.h>
static struct gpio_desc *gpio_num; 
static int my_probe(struct platform_device *pdev)
{
	struct device *dev=&pdev->dev;
	gpio_num=devm_gpiod_get(dev,"led",GPIOD_OUT_LOW);
	if(IS_ERR(gpio_num))
	{
		pr_info("inavlid gpio num\n");
		return -EINVAL;
	}
	for(int i=0;i<5;i++)
	{
		gpiod_set_value(gpio_num,1);
		pr_info("LED TURNED ON\n");
		msleep(500);
		gpiod_set_value(gpio_num,0);
		pr_info("LED TURNED OFF\n");
		msleep(500);
	}
	return 0;
}
static void my_remove(struct platform_device *pdev)
{
	gpiod_set_value(gpio_num,0);
	pr_info("gpio_led module removed\n");
	return ;
}
static const struct of_device_id gpio_led_match_table[]={
	{.compatible="custom,led"},
	{}
};
MODULE_DEVICE_TABLE(of,gpio_led_match_table);

static struct platform_driver gpio_led_driver={
	.probe=my_probe,
	.remove=my_remove,
	.driver={
		.name="gpio_led_driver",
		.of_match_table=gpio_led_match_table,
	},
};
module_platform_driver(gpio_led_driver);
MODULE_AUTHOR("Pavan");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GPIO");
