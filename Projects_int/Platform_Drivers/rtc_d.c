#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/rtc.h>
#include <linux/of.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priya");
MODULE_DESCRIPTION("Simple RTC platform driver for BBG");

#define RTC_BASE_ADDR 0x44E3E000
#define RTC_SIZE      0x1000

struct rtc_platform {
    void __iomem *base;
    struct rtc_device *rtc;
};

static int rtc_platform_read_time(struct device *dev, struct rtc_time *tm)
{
    struct rtc_platform *rtc_p = dev_get_drvdata(dev);
    u32 seconds, minutes, hours, day, month, year;

    seconds = readl(rtc_p->base + 0x00);
    minutes = readl(rtc_p->base + 0x04);
    hours   = readl(rtc_p->base + 0x08);
    day     = readl(rtc_p->base + 0x0C);
    month   = readl(rtc_p->base + 0x10);
    year    = readl(rtc_p->base + 0x14);

    tm->tm_sec  = seconds & 0x3F;
    tm->tm_min  = minutes & 0x3F;
    tm->tm_hour = hours   & 0x1F;
    tm->tm_mday = day     & 0x1F;
    tm->tm_mon  = (month & 0x0F) - 1;
    tm->tm_year = year + 100;   // year since 1900 → e.g. 123 = 2023

    pr_info("RTC read: %d-%d-%d %02d:%02d:%02d\n",
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    return 0;
}

static int rtc_platform_set_time(struct device *dev, struct rtc_time *tm)
{
    struct rtc_platform *rtc_p = dev_get_drvdata(dev);

    writel(tm->tm_sec,     rtc_p->base + 0x00);
    writel(tm->tm_min,     rtc_p->base + 0x04);
    writel(tm->tm_hour,    rtc_p->base + 0x08);
    writel(tm->tm_mday,    rtc_p->base + 0x0C);
    writel(tm->tm_mon + 1, rtc_p->base + 0x10);
    writel(tm->tm_year - 100, rtc_p->base + 0x14);

    pr_info("RTC set: %d-%d-%d %02d:%02d:%02d\n",
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    return 0;
}

static const struct rtc_class_ops rtc_platform_ops = {
    .read_time = rtc_platform_read_time,
    .set_time  = rtc_platform_set_time,
};

static int rtc_platform_probe(struct platform_device *pdev)
{
    struct rtc_platform *rtc_p;
    struct resource *res;

    rtc_p = devm_kzalloc(&pdev->dev, sizeof(*rtc_p), GFP_KERNEL);
    if (!rtc_p)
        return -ENOMEM;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res)
        return -ENODEV;

    rtc_p->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(rtc_p->base))
        return PTR_ERR(rtc_p->base);

    rtc_p->rtc = devm_rtc_device_register(&pdev->dev,
                                          pdev->name,
                                          &rtc_platform_ops,
                                          THIS_MODULE);

    if (IS_ERR(rtc_p->rtc))
        return PTR_ERR(rtc_p->rtc);

    platform_set_drvdata(pdev, rtc_p);
    pr_info("RTC platform driver registered\n");

    return 0;
}

static int rtc_platform_remove(struct platform_device *pdev)
{
    pr_info("RTC platform driver removed\n");
    return 0;
}

static const struct of_device_id rtc_platform_dt_ids[] = {
    { .compatible = "custom,rtc-platform", },
    { }
};
MODULE_DEVICE_TABLE(of, rtc_platform_dt_ids);

static struct platform_driver rtc_platform_driver = {
    .probe  = rtc_platform_probe,
    .remove = rtc_platform_remove,
    .driver = {
        .name           = "rtc_platform",
        .of_match_table = rtc_platform_dt_ids,
    },
};

module_platform_driver(rtc_platform_driver);
