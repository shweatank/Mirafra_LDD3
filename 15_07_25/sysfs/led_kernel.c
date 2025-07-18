#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define LED_PIN 17  // Adjust if needed; remove +512 unless required on your platform

static struct timer_list timer;
static struct kobject *kobj;
static int pattern = 0;
static int led_on = 0;

static void timer_func(struct timer_list *t)
{
    if (pattern == 0)
        return;

    led_on = !led_on;
    gpio_set_value(LED_PIN, led_on);

    if (pattern == 1)
        mod_timer(&timer, jiffies + HZ);     // 1 second
    else if (pattern == 2)
        mod_timer(&timer, jiffies + HZ / 5); // 0.2 seconds
}

static ssize_t pattern_store(struct kobject *kobj, struct kobj_attribute *attr,
                             const char *buf, size_t count)
{
    int new_pattern;
    if (kstrtoint(buf, 10, &new_pattern))
        return -EINVAL;

    pattern = new_pattern;

    del_timer_sync(&timer);

    if (pattern == 0) {
        gpio_set_value(LED_PIN, 0);
        led_on = 0;
    } else {
        mod_timer(&timer, jiffies + ((pattern == 1) ? HZ : HZ / 5));
    }

    return count;
}

static ssize_t pattern_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", pattern);
}

static struct kobj_attribute pattern_attr = __ATTR(pattern, 0664, pattern_show, pattern_store);

static int __init led_init(void)
{
    int ret;

    ret = gpio_request(LED_PIN, "LED");
    if (ret) {
        pr_err("Failed to request GPIO %d\n", LED_PIN);
        return ret;
    }

    ret = gpio_direction_output(LED_PIN, 0);
    if (ret) {
        pr_err("Failed to set GPIO direction\n");
        gpio_free(LED_PIN);
        return ret;
    }

    timer_setup(&timer, timer_func, 0);

    kobj = kobject_create_and_add("led", kernel_kobj);
    if (!kobj) {
        pr_err("Failed to create kobject\n");
        gpio_free(LED_PIN);
        return -ENOMEM;
    }

    ret = sysfs_create_file(kobj, &pattern_attr.attr);
    if (ret) {
        pr_err("Failed to create sysfs file\n");
        kobject_put(kobj);
        gpio_free(LED_PIN);
        return ret;
    }

    pr_info("LED module loaded. Use: echo 1 > /sys/kernel/led/pattern\n");
    return 0;
}

static void __exit led_exit(void)
{
    del_timer_sync(&timer);
    gpio_set_value(LED_PIN, 0);
    gpio_free(LED_PIN);
    sysfs_remove_file(kobj, &pattern_attr.attr);
    kobject_put(kobj);
    pr_info("LED module unloaded.\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("LED Blinking Pattern Control via Sysfs and Timer");

