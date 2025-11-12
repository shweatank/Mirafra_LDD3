// generic_wdt.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>
#include <linux/reboot.h>
#include <linux/ioctl.h>

#define DRIVER_NAME "generic_wdt"
#define WDT_TIMEOUT 10  // default 10 seconds

#define WDT_MAGIC 'W'
#define WDT_START _IO(WDT_MAGIC, 0)
#define WDT_PING  _IO(WDT_MAGIC, 1)
#define WDT_STOP  _IO(WDT_MAGIC, 2)

static struct timer_list wdt_timer;
static DEFINE_MUTEX(wdt_lock);
static bool wdt_active = false;
static unsigned int timeout = WDT_TIMEOUT;

static void wdt_timeout_handler(struct timer_list *t)
{
    pr_info("%s: sk-Watchdog timeout reached! Rebooting system...\n", DRIVER_NAME);
    emergency_restart(); // software reboot
}
static void wdt_start(void)
{
    mutex_lock(&wdt_lock);
    if (!wdt_active) {
        mod_timer(&wdt_timer, jiffies + timeout * HZ);
        wdt_active = true;
        pr_info("%s:sk- Watchdog started (timeout=%u sec)\n", DRIVER_NAME, timeout);
    } else {
        pr_info("%s: sk-Watchdog already running\n", DRIVER_NAME);
    }
    mutex_unlock(&wdt_lock);
}

static void wdt_stop(void)
{
    mutex_lock(&wdt_lock);
    if (wdt_active) {
        del_timer(&wdt_timer);
        wdt_active = false;
        pr_info("%s: sk-Watchdog stopped\n", DRIVER_NAME);
    } else {
        pr_info("%s: sk-Watchdog is not active\n", DRIVER_NAME);
    }
    mutex_unlock(&wdt_lock);
}
static void wdt_ping(void)
{
    mutex_lock(&wdt_lock);
    if (wdt_active) {
        mod_timer(&wdt_timer, jiffies + timeout * HZ);
        pr_info("%s: sk-Watchdog pinged (timer reset)\n", DRIVER_NAME);
    } else {
        pr_info("%s: sk-Ping ignored — watchdog not active\n", DRIVER_NAME);
    }
    mutex_unlock(&wdt_lock);
}

static long wdt_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
    case WDT_START:
        wdt_start();
        break;
    case WDT_PING:
        wdt_ping();
        break;
    case WDT_STOP:
        wdt_stop();
        break;
    default:
        pr_info("%s: sk-Invalid ioctl command\n", DRIVER_NAME);
        return -EINVAL;
    }
    return 0;
}


static const struct file_operations wdt_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = wdt_ioctl,
};

static struct miscdevice wdt_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DRIVER_NAME,
    .fops  = &wdt_fops,
};

static int __init wdt_init(void)
{
    int ret;

    timer_setup(&wdt_timer, wdt_timeout_handler, 0);

    ret = misc_register(&wdt_miscdev);
    if (ret) {
        pr_err("%s: sk-Failed to register device\n", DRIVER_NAME);
        return ret;
    }

    pr_info("%s: sk-Initialized successfully. Timeout=%u sec\n", DRIVER_NAME, timeout);
    return 0;
}

static void __exit wdt_exit(void)
{
    wdt_stop();
    misc_deregister(&wdt_miscdev);
    pr_info("%s:sk- Unloaded.\n", DRIVER_NAME);
}

module_init(wdt_init);
module_exit(wdt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kusuma");
MODULE_DESCRIPTION("Generic Watchdog Timer Driver with Start/Stop/Ping and System Reboot");


