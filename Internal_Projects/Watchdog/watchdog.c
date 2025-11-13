#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/reboot.h>
#include <linux/miscdevice.h>

#define DRIVER_NAME "dummy_wdt"
#define TIMEOUT 10   // seconds

static struct timer_list wdt_timer;
static bool ping_received = false;
static bool active = false;

//---------------------------------------------
// Timer handler — reboot if no ping received
//---------------------------------------------
static void dummy_wdt_timeout(struct timer_list *t)
{
    if (!ping_received && active) {
        pr_alert("%s: Watchdog timeout! Rebooting now...\n", DRIVER_NAME);
        emergency_restart();
        return;
    }

    if (active)
        pr_info("%s: Ping OK, resetting timer\n", DRIVER_NAME);

    ping_received = false;
    mod_timer(&wdt_timer, jiffies + TIMEOUT * HZ);
}

//---------------------------------------------
// Writing to /dev/dummy_wdt counts as ping
//---------------------------------------------
static ssize_t dummy_wdt_write(struct file *file,
                               const char __user *data,
                               size_t len, loff_t *ppos)
{
    if (!active) {
        pr_info("%s: First ping received — watchdog activated\n", DRIVER_NAME);
        active = true;
    }

    ping_received = true;
    return len;
}

//---------------------------------------------
static const struct file_operations dummy_wdt_fops = {
    .owner = THIS_MODULE,
    .write = dummy_wdt_write,
};

static struct miscdevice dummy_wdt_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DRIVER_NAME,
    .fops = &dummy_wdt_fops,
};

//---------------------------------------------
static int __init dummy_wdt_init(void)
{
    int ret;

    pr_info("%s: Initializing (timeout=%d sec)\n", DRIVER_NAME, TIMEOUT);

    ret = misc_register(&dummy_wdt_dev);
    if (ret)
        return ret;

    timer_setup(&wdt_timer, dummy_wdt_timeout, 0);
    mod_timer(&wdt_timer, jiffies + TIMEOUT * HZ);

    pr_info("%s: Device /dev/%s ready (inactive until first ping)\n",
            DRIVER_NAME, DRIVER_NAME);
    return 0;
}

static void __exit dummy_wdt_exit(void)
{
    del_timer_sync(&wdt_timer);
    misc_deregister(&dummy_wdt_dev);
    pr_info("%s: Unloaded\n", DRIVER_NAME);
}

module_init(dummy_wdt_init);
module_exit(dummy_wdt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("Software watchdog for BCM2711");
MODULE_VERSION("1.1");
