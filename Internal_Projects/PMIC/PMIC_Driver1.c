#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

static int virtual_cpu_uv = 1000000;        // microvolts
static int virtual_peripherals_enable = 1;  // 1 = ON, 0 = OFF

static struct kobject *vpmic_kobj;

/* Sysfs show/store for CPU rail */
static ssize_t cpu_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", virtual_cpu_uv);
}

static ssize_t cpu_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
    sscanf(buf, "%d", &virtual_cpu_uv);
    pr_info("Virtual PMIC: CPU rail set to %duV\n", virtual_cpu_uv);
    return count;
}

static struct kobj_attribute cpu_attr =
    __ATTR(cpu_microvolts, 0664, cpu_show, cpu_store);

/* Sysfs show/store for peripheral rail */
static ssize_t periph_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", virtual_peripherals_enable);
}

static ssize_t periph_store(struct kobject *kobj, struct kobj_attribute *attr,
                            const char *buf, size_t count)
{
    sscanf(buf, "%d", &virtual_peripherals_enable);
    pr_info("Virtual PMIC: Peripheral rail %s\n",
            virtual_peripherals_enable ? "enabled" : "disabled");
    return count;
}

static struct kobj_attribute periph_attr =
    __ATTR(peripherals_enable, 0664, periph_show, periph_store);

static int __init vpmic_init(void)
{
    int ret;

    vpmic_kobj = kobject_create_and_add("virtual_pmic", kernel_kobj);
    if (!vpmic_kobj)
        return -ENOMEM;

    ret = sysfs_create_file(vpmic_kobj, &cpu_attr.attr);
    if (ret) goto fail;

    ret = sysfs_create_file(vpmic_kobj, &periph_attr.attr);
    if (ret) goto fail;

    pr_info("Virtual PMIC initialized\n");
    return 0;

fail:
    kobject_put(vpmic_kobj);
    return ret;
}

static void __exit vpmic_exit(void)
{
    kobject_put(vpmic_kobj);
    pr_info("Virtual PMIC removed\n");
}

module_init(vpmic_init);
module_exit(vpmic_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swarna");
MODULE_DESCRIPTION("Software-only Virtual PMIC for BeagleBone Green");
