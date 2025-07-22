#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>

#define UART_PATH "/dev/serial0"
#define UART_BUF_SIZE 128

static struct task_struct *uart_thread;
static struct kobject *uart_kobj;
static char uart_data[UART_BUF_SIZE] = {0}; 
static DEFINE_MUTEX(uart_mutex);            

static ssize_t uart_data_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    ssize_t len;
    mutex_lock(&uart_mutex);
    len = scnprintf(buf, UART_BUF_SIZE, "%s", uart_data);
    mutex_unlock(&uart_mutex);
    return len;
}

static struct kobj_attribute uart_attr = __ATTR_RO(uart_data);

static int uart_reader_thread(void *data) {
    struct file *filp;
    loff_t pos = 0;
    ssize_t ret;
    char tmp[UART_BUF_SIZE];

    filp = filp_open(UART_PATH, O_RDONLY, 0);
    if (IS_ERR(filp)) {
        pr_err("Failed to open %s\n", UART_PATH);
        return PTR_ERR(filp);
    }

    while (!kthread_should_stop()) {
        memset(tmp, 0, sizeof(tmp));
        pos = 0;

        ret = kernel_read(filp, tmp, sizeof(tmp) - 1, &pos);
        if (ret > 0) {
            tmp[ret] = '\0';

            mutex_lock(&uart_mutex);
            strscpy(uart_data, tmp, UART_BUF_SIZE);
            mutex_unlock(&uart_mutex);

            pr_info("UART Read: %s\n", tmp);
        }
        msleep(500);  
    }

    filp_close(filp, NULL);
    return 0;
}

static int __init uart_init(void) {
    int ret;

    
    uart_kobj = kobject_create_and_add("uart", kernel_kobj);
    if (!uart_kobj)
        return -ENOMEM;

    ret = sysfs_create_file(uart_kobj, &uart_attr.attr);
    if (ret) {
        kobject_put(uart_kobj);
        return ret;
    }

    
    uart_thread = kthread_run(uart_reader_thread, NULL, "uart_reader");
    if (IS_ERR(uart_thread)) {
        pr_err("Failed to create UART thread\n");
        sysfs_remove_file(uart_kobj, &uart_attr.attr);
        kobject_put(uart_kobj);
        return PTR_ERR(uart_thread);
    }

    pr_info("UART sysfs module loaded.\n");
    return 0;
}

static void __exit uart_exit(void) {
    if (uart_thread)
        kthread_stop(uart_thread);

    sysfs_remove_file(uart_kobj, &uart_attr.attr);
    kobject_put(uart_kobj);

    pr_info("UART sysfs module unloaded.\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavan");
MODULE_DESCRIPTION("Read UART (/dev/serial0) and expose via sysfs");

