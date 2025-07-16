#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a simple module");

static int my_value;
static struct kobject *myobj;

static ssize_t my_value_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    pr_info("Data sent to userspace is %d\n", my_value);
    return sprintf(buf, "%d\n", my_value);
}

static ssize_t my_value_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%d", &my_value);
    pr_info("Data written to kernel space is %d\n", my_value);
    return count;
}

static struct kobj_attribute myattr = __ATTR_RW(my_value);

static int __init mymod_init(void)
{
    myobj = kobject_create_and_add("my_sysfs_demo", kernel_kobj);
    if (!myobj)
        return -EFAULT;

    if (sysfs_create_file(myobj, &myattr.attr)) {
        kobject_put(myobj);
        return -EFAULT;
    }

    pr_info("sysfs module loaded\n");
    return 0;
}

static void __exit mymod_exit(void)
{
    sysfs_remove_file(myobj, &myattr.attr);
    kobject_put(myobj);
    pr_info("sysfs module unloaded\n");
}

module_init(mymod_init);
module_exit(mymod_exit);

