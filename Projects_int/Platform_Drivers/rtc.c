#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/rtc.h>
#include <linux/time.h>

static int __init rtc_example_init(void)
{
    struct rtc_device *rtc;
    struct rtc_time tm;
    int ret;

    pr_info("rtc_example: module loaded\n");

    /* Open the RTC device (rtc0 is usually the hardware clock) */
    rtc = rtc_class_open("rtc0");
    if (!rtc) {
        pr_err("rtc_example: unable to open rtc0\n");
        return -ENODEV;
    }

    /* Read current RTC time */
    ret = rtc_read_time(rtc, &tm);
    if (ret) {
        pr_err("rtc_example: failed to read time\n");
        rtc_class_close(rtc);
        return ret;
    }

    /* Print the current RTC time to kernel log */
    pr_info("rtc_example: Current RTC Time: %02d-%02d-%04d %02d:%02d:%02d\n",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec);

    rtc_class_close(rtc);
    return 0;
}

static void __exit rtc_example_exit(void)
{
    pr_info("rtc_example: module unloaded\n");
}

module_init(rtc_example_init);
module_exit(rtc_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priya");
MODULE_DESCRIPTION("Simple RTC Example Driver");
