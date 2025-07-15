// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple calculator using sysfs");
MODULE_VERSION("1.0");

static struct kobject *calc_kobj;

static int calc_result = 0;

// Helper: simple calculator (supports only +, -, *, /)
static int simple_calc(const char *expr, int *result)
{
    int a, b;
    char op;

    // parse format "a op b", e.g. "5 + 3"
    if (sscanf(expr, "%d %c %d", &a, &op, &b) != 3)
        return -EINVAL;

    switch (op) {
    case '+':
        *result = a + b;
        break;
    case '-':
        *result = a - b;
        break;
    case '*':
        *result = a * b;
        break;
    case '/':
        if (b == 0)
            return -EINVAL;  // division by zero
        *result = a / b;
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

// Show result: read from /sys/kernel/calc/result
static ssize_t result_show(struct kobject *kobj, struct kobj_attribute *attr,
                           char *buf)
{
    return sprintf(buf, "%d\n", calc_result);
}

// Store expression: write to /sys/kernel/calc/expression
static ssize_t expression_store(struct kobject *kobj, struct kobj_attribute *attr,
                                const char *buf, size_t count)
{
    char expr[64];

    if (count >= sizeof(expr))
        return -EINVAL;

    // Copy expression, strip trailing newline
    strncpy(expr, buf, count);
    expr[count] = '\0';
    if (expr[count - 1] == '\n')
        expr[count - 1] = '\0';

    if (simple_calc(expr, &calc_result) < 0)
        return -EINVAL;

    pr_info("calc: expression '%s', result %d\n", expr, calc_result);
    return count;
}

// Define attributes
static struct kobj_attribute result_attr = __ATTR_RO(result);
static struct kobj_attribute expression_attr = __ATTR_WO(expression);

// Attribute group
static struct attribute *attrs[] = {
    &result_attr.attr,
    &expression_attr.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static int __init calc_init(void)
{
    int retval;

    calc_kobj = kobject_create_and_add("calc", kernel_kobj);
    if (!calc_kobj)
        return -ENOMEM;

    retval = sysfs_create_group(calc_kobj, &attr_group);
    if (retval)
        kobject_put(calc_kobj);

    pr_info("calc: module loaded\n");
    return retval;
}

static void __exit calc_exit(void)
{
    kobject_put(calc_kobj);
    pr_info("calc: module unloaded\n");
}

module_init(calc_init);
module_exit(calc_exit);

