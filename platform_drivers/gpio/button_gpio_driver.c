#include<linux/module.h>
#include<linux/of.h>
#include<linux/gpio.h>
#include<linux/of_gpio.h>
#include<linux/platform_device.h>
#include<linux/interrupt.h>
static struct gpio_desc *button_num;
static int irq_num;
static irqreturn_t my_isr(int irq,void *devm)
{
	pr_info("my_isr called\n");
	return IRQ_HANDLED;
}
static int my_probe(struct platform_device *pdev)
{
	struct device *dev=&pdev->dev;
	pr_info("probe function called\n");
	button_num=devm_gpiod_get(dev,NULL,0);
	if(IS_ERR(button_num))
	{
		pr_info("invalid gpio\n");
		return -EINVAL;
	}
	irq_num=gpiod_to_irq(button_num);
	if(irq_num<0)
	{
		pr_info("irq failed to get the irq number\n");
		return irq_num;
	}
	if(devm_request_irq(dev,irq_num,my_isr,IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,"my_isr",NULL))
	{
		pr_info("failed to register irq\n");
		return -1;
	}
	pr_info("button irq registered\n");
	return 0;
}
static void my_remove(struct platform_device *pdev)
{
	pr_info("module removed successfully\n");
	return ;
}

static const struct of_device_id button_gpio_table[]={
	{.compatible="rpi,button_gpio",},
	{},
};

MODULE_DEVICE_TABLE(of,button_gpio_table);
static struct platform_driver button_gpio_declare={
	.probe=my_probe,
	.remove=my_remove,
	.driver={
		.name="button_gpio_driver",
		.of_match_table=button_gpio_table,
	},
};
module_platform_driver(button_gpio_declare);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavan");

