// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Sysfs with probe/store using platform driver on x86");
MODULE_VERSION("1.0");

struct my_driver_data {
    int foo;
    struct kobject *kobj;
};

static ssize_t foo_show(struct device *dev,
                        struct device_attribute *attr, char *buf)
{
    struct my_driver_data *data = dev_get_drvdata(dev);
    return sprintf(buf, "%d\n", data->foo);
}

static ssize_t foo_store(struct device *dev,
                         struct device_attribute *attr,
                         const char *buf, size_t count)
{
    struct my_driver_data *data = dev_get_drvdata(dev);
    int val;

    if (kstrtoint(buf, 10, &val) < 0)
        return -EINVAL;

    data->foo = val;
    return count;
}

/* Define device attribute */
static DEVICE_ATTR_RW(foo);

/* Attribute list */
static struct attribute *my_attrs[] = {
    &dev_attr_foo.attr,
    NULL,
};

static const struct attribute_group my_attr_group = {
    .attrs = my_attrs,
};

/* Probe */
static int my_probe(struct platform_device *pdev)
{
    struct my_driver_data *data;

    pr_info("sysfs_platform_driver: probe called\n");

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->foo = 0; // default
    platform_set_drvdata(pdev, data);

    return sysfs_create_group(&pdev->dev.kobj, &my_attr_group);
}

/* Remove */
static int my_remove(struct platform_device *pdev)
{
    pr_info("sysfs_platform_driver: remove called\n");
    sysfs_remove_group(&pdev->dev.kobj, &my_attr_group);
    return 0;
}

/* Platform driver */
static struct platform_driver my_platform_driver = {
    .driver = {
        .name = "my_device",
    },
    .probe = my_probe,
    .remove = my_remove,
};

/* Platform device */
static struct platform_device *my_platform_device;

/* Init */
static int __init sysfs_platform_init(void)
{
    int ret;

    ret = platform_driver_register(&my_platform_driver);
    if (ret)
        return ret;

    my_platform_device = platform_device_register_simple("my_device", -1, NULL, 0);
    if (IS_ERR(my_platform_device)) {
        platform_driver_unregister(&my_platform_driver);
        return PTR_ERR(my_platform_device);
    }

    pr_info("sysfs_platform_driver: loaded\n");
    return 0;
}

/* Exit */
static void __exit sysfs_platform_exit(void)
{
    platform_device_unregister(my_platform_device);
    platform_driver_unregister(&my_platform_driver);
    pr_info("sysfs_platform_driver: unloaded\n");
}

module_init(sysfs_platform_init);
module_exit(sysfs_platform_exit);
