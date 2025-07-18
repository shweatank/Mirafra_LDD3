#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define LED_PIN (17+512)

static struct timer_list timer;
static struct kobject *kobj;
static int pattern = 0;
static int led_on = 0;

static void timer_func(struct timer_list *t)
{
    if (pattern == 0) return;
    
    led_on = !led_on;
    gpio_set_value(LED_PIN, led_on);
    
    if (pattern == 1)
        mod_timer(&timer, jiffies + HZ);      // 1 sec
    else if (pattern == 2)
        mod_timer(&timer, jiffies + HZ/5);    // 0.2 sec
}

static ssize_t pattern_store(struct kobject *kobj, struct kobj_attribute *attr,
                            const char *buf, size_t count)
{
    sscanf(buf, "%d", &pattern);
    
    del_timer(&timer);
    
    if (pattern == 0) {
        gpio_set_value(LED_PIN, 0);
    } else {
        timer_func(&timer);
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
    gpio_request(LED_PIN, "LED");
    gpio_direction_output(LED_PIN, 0);
    
    timer_setup(&timer, timer_func, 0);
    
    kobj = kobject_create_and_add("led", kernel_kobj);
   // sysfs_create_file(kobj, &pattern_attr.attr);
    if (sysfs_create_file(kobj, &pattern_attr.attr)) {
    pr_err("Failed to create sysfs file\n");
    return -ENOMEM;
}

    
    printk("LED module loaded. Use: echo 1 > /sys/kernel/led/pattern\n");
    return 0;
}

static void __exit led_exit(void)
{
    del_timer(&timer);
    gpio_set_value(LED_PIN, 0);
    gpio_free(LED_PIN);
    sysfs_remove_file(kobj, &pattern_attr.attr);
    kobject_put(kobj);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
