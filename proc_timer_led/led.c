#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/timer.h>

#define PROC_NAME "procfs_example"
#define BUFFER_SIZE 128
#define GPIO_LED 17+512

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha Kadali");
MODULE_DESCRIPTION("Raspberry Pi LED blink using timer and /proc interface");
MODULE_VERSION("1.0");

static char procfs_buffer[BUFFER_SIZE];
static int led_state = 0;
static int blinking = 0;

static struct timer_list led_timer;

static ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *pos);

static void led_timer_callback(struct timer_list *t)
{
    led_state = !led_state;
    gpio_set_value(GPIO_LED, led_state);

    if (blinking) {
        mod_timer(&led_timer, jiffies + msecs_to_jiffies(1000)); 
    }
}

// Read handler
static ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos)
{
    int len;

    if (*pos > 0)
        return 0;

    if (blinking)
        len = snprintf(procfs_buffer, BUFFER_SIZE, "LED is blinking. State: %s\n", led_state ? "ON" : "OFF");
    else
        len = snprintf(procfs_buffer, BUFFER_SIZE, "LED is NOT blinking. State: %s\n", led_state ? "ON" : "OFF");

    if (copy_to_user(user_buf, procfs_buffer, len))
        return -EFAULT;

    *pos = len;
    return len;
}

static ssize_t proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *pos)
{
        int value;

    if (count >= BUFFER_SIZE)
        return -EINVAL;

    memset(procfs_buffer, 0, BUFFER_SIZE);

    if (copy_from_user(procfs_buffer, user_buf, count))
        return -EFAULT;

    if (kstrtoint(procfs_buffer, 10, &value) < 0)
        return -EINVAL;

    blinking = value ? 1 : 0;
     if (blinking)
     {
        pr_info("LED blinking started.\n");
        mod_timer(&led_timer, jiffies + msecs_to_jiffies(1000)); // Start blinking
    } 
     else 
    {
        pr_info("LED blinking stopped.\n");
        del_timer(&led_timer);              // Stop blinking
        gpio_set_value(GPIO_LED, 0);        // Turn off LED
        led_state = 0;
    }
    return count;
}

// Proc file operations
static const struct proc_ops proc_file_ops = {
    .proc_read  = proc_read,
    .proc_write = proc_write,
};

// Module init
static int __init procfs_driver_init(void)
{
    struct proc_dir_entry *entry;

    if (!gpio_is_valid(GPIO_LED)) {
        pr_err("Invalid GPIO %d\n", GPIO_LED);
        return -ENODEV;
    }

    if (gpio_request(GPIO_LED, "LED_GPIO")) {
        pr_err("GPIO %d request failed\n", GPIO_LED);
        return -EBUSY;
    }

    gpio_direction_output(GPIO_LED, 0);

    timer_setup(&led_timer, led_timer_callback, 0);

    entry = proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);
    if (!entry) {
        gpio_free(GPIO_LED);
        pr_err("Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    pr_info("procfs_timer_led: Loaded. /proc/%s ready. Write START/STOP.\n", PROC_NAME);
    return 0;
}

// Module exit
static void __exit procfs_driver_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    del_timer_sync(&led_timer);
    gpio_set_value(GPIO_LED, 0);
    gpio_free(GPIO_LED);
    pr_info("procfs_timer_led: Unloaded. LED off. GPIO freed.\n");
}

module_init(procfs_driver_init);
module_exit(procfs_driver_exit);

