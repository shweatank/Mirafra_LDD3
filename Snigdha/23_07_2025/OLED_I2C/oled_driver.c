#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/fb.h>

static struct gpio_desc *led_gpio;
static struct task_struct *blink_thread;

static int led_blink_fn(void *data)
{
    struct fb_info *info = framebuffer_alloc(0, NULL);
    while (!kthread_should_stop()) {
        gpiod_set_value(led_gpio, 1);
        pr_info("LED ON\n");
        msleep(500);
        gpiod_set_value(led_gpio, 0);
        pr_info("LED OFF\n");
        msleep(500);
    }
    return 0;
}

static int led_oled_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;

    led_gpio = gpiod_get(dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(led_gpio))
        return dev_err_probe(dev, PTR_ERR(led_gpio), "Failed to get LED GPIO\n");

    blink_thread = kthread_run(led_blink_fn, NULL, "led_blink_thread");
    if (IS_ERR(blink_thread)) {
        gpiod_put(led_gpio);
        return dev_err_probe(dev, PTR_ERR(blink_thread), "Failed to create thread\n");
    }

    dev_info(dev, "Platform LED+OLED driver started\n");
    return 0;
}

static void led_oled_remove(struct platform_device *pdev)
{
    kthread_stop(blink_thread);
    gpiod_put(led_gpio);
    pr_info("Platform driver removed\n");
}

static const struct of_device_id led_oled_dt_ids[] = {
    { .compatible = "rpi,gpio-led-blink" },
    { }
};
MODULE_DEVICE_TABLE(of, led_oled_dt_ids);

static struct platform_driver led_oled_driver = {
    .driver = {
        .name = "led_oled_driver",
        .of_match_table = led_oled_dt_ids,
    },
    .probe = led_oled_probe,
    .remove = led_oled_remove,
};

module_platform_driver(led_oled_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("LED Blinker + OLED Init Kernel Driver");

