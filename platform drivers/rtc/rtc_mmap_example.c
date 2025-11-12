#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>
#include <linux/of.h>
#include <linux/of_device.h>

static int rtc_example_probe(struct platform_device *pdev)
{
    struct rtc_device *rtc;
    struct rtc_time tm;
    int ret;

    pr_info("RTC Example: Probe called\n");

    /* Open RTC device by name (usually rtc0) */
    rtc = rtc_class_open("rtc0");
    if (!rtc) {
        pr_err("RTC Example: Failed to open rtc0\n");
        return -ENODEV;
    }

    /* Read RTC time */
    ret = rtc_read_time(rtc, &tm);
    if (ret) {
        pr_err("RTC Example: rtc_read_time failed\n");
        rtc_class_close(rtc);
        return ret;
    }

    pr_info("RTC Example: Current time %04d-%02d-%02d %02d:%02d:%02d\n",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec);

    rtc_class_close(rtc);

    return 0;
}

static int rtc_example_remove(struct platform_device *pdev)
{
    pr_info("RTC Example: Remove called\n");
    return 0;
}

static const struct of_device_id rtc_example_of_match[] = {
    { .compatible = "ti,am335x-rtc", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtc_example_of_match);

static struct platform_driver rtc_example_driver = {
    .driver = {
        .name = "rtc_example",
        .of_match_table = rtc_example_of_match,
    },
    .probe = rtc_example_probe,
    .remove = rtc_example_remove,
};

module_platform_driver(rtc_example_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("AM335x On-Chip RTC Example for 4.19-ti-r42");

