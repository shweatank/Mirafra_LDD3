#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/init.h>

#define GPIO_LED    (17 + 512)   // BCM GPIO 17 for LED output
#define GPIO_BUTTON (18 + 512)   // BCM GPIO 18 for input button with IRQ

static int irq_number;
static bool led_on = false;

// Interrupt handler: toggle LED state
static irqreturn_t gpio_irq_handler(int irq, void *dev_id) {
    led_on = !led_on;
    gpio_set_value(GPIO_LED, led_on);
    printk(KERN_INFO "LED: Toggled to %d by interrupt\n", led_on);
    return IRQ_HANDLED;
}

static int __init led_gpio_interrupt_init(void) {
    int ret;

    printk(KERN_INFO "LED Interrupt: Initializing...\n");

    // Request LED GPIO and set as output low
    ret = gpio_request(GPIO_LED, "LED_GPIO");
    if (ret) {
        printk(KERN_ALERT "LED Interrupt: Failed to request GPIO_LED %d\n", GPIO_LED);
        return ret;
    }
    gpio_direction_output(GPIO_LED, 0);
    led_on = false;

    // Request Button GPIO and set as input
    ret = gpio_request(GPIO_BUTTON, "BUTTON_GPIO");
    if (ret) {
        printk(KERN_ALERT "LED Interrupt: Failed to request GPIO_BUTTON %d\n", GPIO_BUTTON);
        gpio_free(GPIO_LED);
        return ret;
    }
    gpio_direction_input(GPIO_BUTTON);

    // Get IRQ number for button GPIO
    irq_number = gpio_to_irq(GPIO_BUTTON);
    if (irq_number < 0) {
        printk(KERN_ALERT "LED Interrupt: Failed to get IRQ number for GPIO_BUTTON\n");
        gpio_free(GPIO_LED);
        gpio_free(GPIO_BUTTON);
        return irq_number;
    }
    printk(KERN_INFO "LED Interrupt: IRQ number for button GPIO is %d\n", irq_number);

    // Request IRQ on falling edge (button press)
    ret = request_irq(irq_number,
                      gpio_irq_handler,
                      IRQF_TRIGGER_FALLING,
                      "led_gpio_interrupt",
                      NULL);
    if (ret) {
        printk(KERN_ALERT "LED Interrupt: Unable to request IRQ\n");
        gpio_free(GPIO_LED);
        gpio_free(GPIO_BUTTON);
        return ret;
    }

    printk(KERN_INFO "LED Interrupt: Module loaded successfully\n");
    return 0;
}

static void __exit led_gpio_interrupt_exit(void) {
    gpio_set_value(GPIO_LED, 0); // Turn off LED on exit
    free_irq(irq_number, NULL);
    gpio_free(GPIO_LED);
    gpio_free(GPIO_BUTTON);
    printk(KERN_INFO "LED Interrupt: Module unloaded\n");
}

module_init(led_gpio_interrupt_init);
module_exit(led_gpio_interrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba");
MODULE_DESCRIPTION("GPIO LED Blink using Interrupt only");

