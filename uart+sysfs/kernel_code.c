#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Sysfs UART Communication Example");
MODULE_VERSION("1.0");

static struct kobject *sysfs_kobj;

static char tx_buf[100] = "Hello Pi\n";  // Data to send to UART
static char rx_buf[100];                 // Data received from UART

// Read-only attribute for tx_data (read by user-space UART app)
static ssize_t tx_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%s", tx_buf);
}

// Write-only attribute for rx_data (written by user-space UART app)
static ssize_t rx_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    snprintf(rx_buf, sizeof(rx_buf), "%.*s", (int)count, buf);
    pr_info("UART RX → Kernel: %s\n", rx_buf);
    return count;
}

// Optional: read back last received data
static ssize_t rx_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%s", rx_buf);
}

// Write-only attribute for user to update tx_buf (optional)
static ssize_t tx_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    snprintf(tx_buf, sizeof(tx_buf), "%.*s", (int)count, buf);
    pr_info("Sysfs updated TX: %s\n", tx_buf);
    return count;
}

static struct kobj_attribute tx_attr = __ATTR(tx_data, 0664, tx_show, tx_store);
static struct kobj_attribute rx_attr = __ATTR(rx_data, 0664, rx_show, rx_store);

static int __init sysfs_uart_init(void) {
    int ret;

    sysfs_kobj = kobject_create_and_add("my_sysfs", kernel_kobj);
    if (!sysfs_kobj)
        return -ENOMEM;

    ret = sysfs_create_file(sysfs_kobj, &tx_attr.attr);
    if (ret)
        return ret;

    ret = sysfs_create_file(sysfs_kobj, &rx_attr.attr);
    if (ret)
        return ret;

    pr_info("Sysfs UART module loaded.\n");
    return 0;
}

static void __exit sysfs_uart_exit(void) {
    kobject_put(sysfs_kobj);
    pr_info("Sysfs UART module unloaded.\n");
}

module_init(sysfs_uart_init);
module_exit(sysfs_uart_exit);

