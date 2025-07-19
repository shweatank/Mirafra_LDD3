#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio/consumer.h>  // For gpiod APIs
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#define GPIO_LED    17
#define GPIO_BUTTON 27

static unsigned int irq_number;
static struct gpio_desc *led_desc;
static struct gpio_desc *button_desc;

static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    static bool led_on = false;

    led_on = !led_on;
    gpiod_set_value(led_desc, led_on);
    printk(KERN_INFO "GPIO IRQ: LED is now %s\n", led_on ? "ON" : "OFF");

    return IRQ_HANDLED;
}

static int __init gpio_irq_init(void)
{
    int ret;

    printk(KERN_INFO "GPIO IRQ Module Init (gpiod)\n");

    // Request LED GPIO as output
    led_desc = gpiod_get(NULL, "GPIO17", GPIOD_OUT_LOW);
    if (IS_ERR(led_desc)) {
        pr_err("Failed to get LED GPIO\n");
        return PTR_ERR(led_desc);
    }

    // Request Button GPIO as input
    button_desc = gpiod_get(NULL, "GPIO27", GPIOD_IN);
    if (IS_ERR(button_desc)) {
        pr_err("Failed to get Button GPIO\n");
        gpiod_put(led_desc);
        return PTR_ERR(button_desc);
    }

    // Map GPIO to IRQ
    irq_number = gpiod_to_irq(button_desc);
    if (irq_number < 0) {
        pr_err("Failed to map GPIO to IRQ\n");
        gpiod_put(led_desc);
        gpiod_put(button_desc);
        return irq_number;
    }

    pr_info("Mapped Button GPIO to IRQ %d\n", irq_number);

    ret = request_irq(irq_number,
                      gpio_irq_handler,
                      IRQF_TRIGGER_FALLING,
                      "gpio_irq_handler",
                      NULL);

    if (ret) {
        pr_err("Failed to request IRQ\n");
        gpiod_put(led_desc);
        gpiod_put(button_desc);
        return ret;
    }

    return 0;
}

static void __exit gpio_irq_exit(void)
{
    printk(KERN_INFO "GPIO IRQ Module Exit\n");

    free_irq(irq_number, NULL);
    gpiod_set_value(led_desc, 0);  // Turn off LED

    gpiod_put(led_desc);
    gpiod_put(button_desc);
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("GPIO Interrupt Example (gpiod API)");

