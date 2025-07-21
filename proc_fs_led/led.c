// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>       // Needed for all modules
#include <linux/kernel.h>       // Needed for KERN_INFO
#include <linux/proc_fs.h>      // For procfs APIs
#include <linux/uaccess.h>      // For copy_from_user and copy_to_user
#include <linux/gpio.h>

#define PROC_NAME "procfs_example"
#define BUFFER_SIZE 128
#define GPIO_LED 17+512

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple /proc driver LED ON and OFF");
MODULE_VERSION("1.0");

static int led_state = 0;
static char procfs_buffer[BUFFER_SIZE];

// Read handler - returns "LED is ON" or "LED is OFF"
ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos)
{
    int len;

    if (*pos > 0)
        return 0;

    if (led_state)
        len = snprintf(procfs_buffer, BUFFER_SIZE, "LED is ON\n");
    else
        len = snprintf(procfs_buffer, BUFFER_SIZE, "LED is OFF\n");

    if (copy_to_user(user_buf, procfs_buffer, len))
        return -EFAULT;

    *pos = len;
    return len;
}
// Write handler
ssize_t proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *pos)
{
    if (count > BUFFER_SIZE)
        return -EINVAL;

    memset(procfs_buffer, 0, BUFFER_SIZE);

    if (copy_from_user(procfs_buffer, user_buf, count))
        return -EFAULT;

    if (kstrtoint(procfs_buffer, 10, &led_state) < 0)
        return -EINVAL;

    if (led_state)
        gpio_set_value(GPIO_LED, 1);  // Turn LED ON
    else
        gpio_set_value(GPIO_LED, 0);  // Turn LED OFF

    return count;
}

static const struct proc_ops proc_file_ops = {
    .proc_read  = proc_read,   // Assign read handler
    .proc_write = proc_write,  // Assign write handler
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

    gpio_direction_output(GPIO_LED, 0); // Set as output and turn OFF

    entry = proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);
    if (!entry) {
        gpio_free(GPIO_LED);
        pr_err("procfs_driver: Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    pr_info("procfs_driver: Loaded. /proc/%s created. GPIO %d ready.\n", PROC_NAME, GPIO_LED);
    return 0;
}

// Module exit
static void __exit procfs_driver_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    gpio_set_value(GPIO_LED, 0); // Ensure LED is off
    gpio_free(GPIO_LED);
    pr_info("procfs_driver: Unloaded. /proc/%s removed. GPIO %d released.\n", PROC_NAME, GPIO_LED);
}

module_init(procfs_driver_init);
module_exit(procfs_driver_exit);
