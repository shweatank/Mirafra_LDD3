// sysfs_module.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

static char data_buf[100] = "initial";
static struct kobject *example_kobj;

static ssize_t data_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%s\n", data_buf);
}

static ssize_t data_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    snprintf(data_buf, sizeof(data_buf), "%s", buf);
    return count;
}

static struct kobj_attribute data_attr = __ATTR(my_data, 0660, data_show, data_store);

static int __init example_init(void) {
    example_kobj = kobject_create_and_add("my_sysfs", kernel_kobj);
    if (!example_kobj) return -ENOMEM;
    return sysfs_create_file(example_kobj, &data_attr.attr);
}

static void __exit example_exit(void) {
    kobject_put(example_kobj);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");

