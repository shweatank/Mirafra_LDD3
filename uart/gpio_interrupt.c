#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#define GPIO_IN  17  // GPIO pin connected to the button (input)
#define GPIO_OUT 18  // GPIO pin connected to the LED (output)

static int irq_number;

static irqreturn_t irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "IRQ: Falling edge detected on GPIO %d\n", GPIO_IN);

    // Blink the LED for 200ms
    gpio_set_value(GPIO_OUT, 1);
    msleep(200);
    gpio_set_value(GPIO_OUT, 0);

    return IRQ_HANDLED;
}

static int __init irq_module_init(void)
{
    int ret;

    // Validate GPIOs
    if (!gpio_is_valid(GPIO_IN)) {
        printk(KERN_ERR "Invalid GPIO: %d (input)\n", GPIO_IN);
        return -EINVAL;
    }

    if (!gpio_is_valid(GPIO_OUT)) {
        printk(KERN_ERR "Invalid GPIO: %d (output)\n", GPIO_OUT);
        return -EINVAL;
    }

    // Request input GPIO
    if ((ret = gpio_request(GPIO_IN, "gpio_in")) < 0)
        return ret;

    gpio_direction_input(GPIO_IN);
    gpio_set_debounce(GPIO_IN, 200); // optional

    // Request output GPIO for LED
    if ((ret = gpio_request(GPIO_OUT, "gpio_out")) < 0) {
        gpio_free(GPIO_IN);
        return ret;
    }

    gpio_direction_output(GPIO_OUT, 0);

    // Map GPIO to IRQ
    irq_number = gpio_to_irq(GPIO_IN);
    if (irq_number < 0) {
        ret = irq_number;
        goto fail_free;
    }

    // Request IRQ on falling edge
    ret = request_irq(irq_number,
                      irq_handler,
                      IRQF_TRIGGER_FALLING,
                      "gpio_irq_handler",
                      NULL);

    if (ret) {
        printk(KERN_ERR "Failed to request IRQ %d\n", irq_number);
        goto fail_free;
    }

    printk(KERN_INFO "Module loaded. IRQ %d on GPIO %d (falling edge)\n", irq_number, GPIO_IN);
    return 0;

fail_free:
    gpio_free(GPIO_OUT);
    gpio_free(GPIO_IN);
    return ret;
}

static void __exit irq_module_exit(void)
{
    free_irq(irq_number, NULL);
    gpio_set_value(GPIO_OUT, 0);
    gpio_free(GPIO_OUT);
    gpio_free(GPIO_IN);
    printk(KERN_INFO "IRQ module unloaded.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavan");
MODULE_DESCRIPTION("External falling-edge interrupt to blink LED using GPIO");
MODULE_VERSION("1.1");

module_init(irq_module_init);
module_exit(irq_module_exit);

