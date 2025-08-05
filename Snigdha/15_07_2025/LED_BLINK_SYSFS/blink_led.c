#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define GPIO_LED 17  // LED pin on Raspberry Pi

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba + chatgpt");
MODULE_DESCRIPTION("Blink LED via sysfs using GPIO set/clear");
MODULE_VERSION("1.0");

static struct timer_list blink_timer;
static struct kobject *blink_kobj;

static bool blinking = false;
static int led_state = 0;

// Timer callback toggles LED
static void timer_callback(struct timer_list *t)
{
    if (!blinking)
        return;

    led_state = !led_state;
    gpio_set_value(GPIO_LED, led_state);
    mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500));
}

// Show sysfs attribute
static ssize_t enabled_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", blinking);
}

// Store sysfs attribute (write 1 or 0)
static ssize_t enabled_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int val;

    if (kstrtoint(buf, 10, &val) < 0)
        return -EINVAL;

    if (val == 1 && !blinking) {
        blinking = true;
        mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500));
        pr_info("LED blinking enabled\n");
    } else if (val == 0 && blinking) {
        blinking = false;
        del_timer_sync(&blink_timer);
        gpio_set_value(GPIO_LED, 0);
        pr_info("LED blinking disabled\n");
    }

    return count;
}

// Sysfs attribute structure
static struct kobj_attribute enabled_attr = __ATTR(enabled, 0660, enabled_show, enabled_store);

// Attribute group
static struct attribute *attrs[] = {
    &enabled_attr.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .name = "blink_sysfs",
    .attrs = attrs,
};

// Module Init
static int __init blink_init(void)
{
    int ret;

    // Request LED GPIO
    if ((ret = gpio_request(GPIO_LED, "LED_GPIO")) < 0) {
        pr_err("Failed to request GPIO\n");
        return ret;
    }
    gpio_direction_output(GPIO_LED, 0);

    // Setup timer
    timer_setup(&blink_timer, timer_callback, 0);

    // Create sysfs entry
    blink_kobj = kobject_create_and_add("blink_sysfs", kernel_kobj);
    if (!blink_kobj)
        return -ENOMEM;

    ret = sysfs_create_group(blink_kobj, &attr_group);
    if (ret) {
        kobject_put(blink_kobj);
        gpio_free(GPIO_LED);
        return ret;
    }

    pr_info("Blink module loaded. Use /sys/kernel/blink_sysfs/enabled to control LED\n");
    return 0;
}

// Module Exit
static void __exit blink_exit(void)
{
    del_timer_sync(&blink_timer);
    gpio_set_value(GPIO_LED, 0);
    gpio_free(GPIO_LED);
    sysfs_remove_group(blink_kobj, &attr_group);
    kobject_put(blink_kobj);
    pr_info("Blink module unloaded\n");
}

module_init(blink_init);
module_exit(blink_exit);

