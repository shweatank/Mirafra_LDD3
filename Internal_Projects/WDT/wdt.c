// bcm2711_wdt_safe.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define DRIVER_NAME "bcm2711_wdt"

// BCM2711 peripheral base
#define PERIPH_BASE    0xFE000000
#define PM_RSTC_OFFSET 0x1C
#define PM_WDOG_OFFSET 0x24

#define PM_PASSWORD    0x5A000000
#define PM_RSTC_WRCFG_FULL_RESET 0x00000020

// User commands
#define CMD_START 1
#define CMD_STOP  2
#define CMD_PING  3

static void __iomem *pm_base;
static void __iomem *pm_wdog;
static void __iomem *pm_rstc;

static long bcm2711_wdt_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case CMD_START:
            // Safe long timeout for testing (no immediate reset)
            iowrite32(PM_PASSWORD | 0xFFFFFF, pm_wdog);
            pr_info("BCM2711 WDT started (safe mode)\n");
            break;

        case CMD_PING:
            iowrite32(PM_PASSWORD | 0xFFFFFF, pm_wdog);
            pr_info("BCM2711 WDT pinged\n");
            break;

        case CMD_STOP:
            // Stop: reload counter to max
            iowrite32(PM_PASSWORD | 0xFFFFFF, pm_wdog);
            pr_info("BCM2711 WDT stopped (approximate)\n");
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

static const struct file_operations bcm2711_wdt_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = bcm2711_wdt_ioctl,
};

static struct miscdevice bcm2711_wdt_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "bcm2711_wdt",
    .fops = &bcm2711_wdt_fops,
};

static int __init bcm2711_wdt_init(void)
{
    // Map PM registers (4KB)
    pm_base = ioremap(PERIPH_BASE + 0x100000, 0x1000);
    if (!pm_base) {
        pr_err("Failed to map PM registers\n");
        return -ENOMEM;
    }

    pm_wdog = pm_base + PM_WDOG_OFFSET;
    pm_rstc = pm_base + PM_RSTC_OFFSET;

    if (misc_register(&bcm2711_wdt_dev)) {
        iounmap(pm_base);
        pr_err("Failed to register misc device\n");
        return -1;
    }

    pr_info("BCM2711 WDT safe driver loaded\n");
    return 0;
}

static void __exit bcm2711_wdt_exit(void)
{
    misc_deregister(&bcm2711_wdt_dev);
    iounmap(pm_base);
    pr_info("BCM2711 WDT safe driver unloaded\n");
}

module_init(bcm2711_wdt_init);
module_exit(bcm2711_wdt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Revathi");
MODULE_DESCRIPTION("BCM2711 Custom Watchdog Driver (Safe Test Mode)");
