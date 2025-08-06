// x86_B_kernel.c – Final version with UART TX/RX over /dev/ttyUSB0
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/fcntl.h>

#define UART_DEV        "/dev/ttyUSB0"
#define DEVICE_NAME_TX  "uartchar"
#define DEVICE_NAME_RX  "uartalert"

static dev_t dev_tx, dev_rx;
static struct cdev cdev_tx, cdev_rx;
static struct class *uart_class;
static struct file *uart_fp;
static struct task_struct *rx_thread;

static char alert_char = 0;
static DEFINE_MUTEX(alert_lock);
static DECLARE_WAIT_QUEUE_HEAD(alert_wq);
static int alert_ready = 0;

// UART RX thread
static int uart_rx_fn(void *data) {
    char ch;
    loff_t pos = 0;

    while (!kthread_should_stop()) {
        if (!uart_fp)
            break;

        if (kernel_read(uart_fp, &ch, 1, &pos) == 1) {
            if (ch == 'x' || ch == 'z') {
                mutex_lock(&alert_lock);
                alert_char = ch;
                alert_ready = 1;
                mutex_unlock(&alert_lock);
                wake_up_interruptible(&alert_wq);
            }
        }
        msleep(100);
    }
    return 0;
}

// TX file ops
static ssize_t uart_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos) {
    char *kbuf = kmalloc(len + 1, GFP_KERNEL);
    loff_t pos = 0;
    ssize_t written = 0;

    if (!kbuf)
        return -ENOMEM;
    if (copy_from_user(kbuf, buf, len)) {
        kfree(kbuf);
        return -EFAULT;
    }

    kbuf[len] = '\0';
    pr_info("[uart_write] Writing to UART: %s\n", kbuf);

    if (uart_fp)
        written = kernel_write(uart_fp, kbuf, len, &pos);

    kfree(kbuf);
    return written > 0 ? len : written;
}

static struct file_operations fops_tx = {
    .owner = THIS_MODULE,
    .write = uart_write,
};

// RX file ops
static ssize_t uart_read(struct file *file, char __user *buf, size_t len, loff_t *ppos) {
    char kbuf;

    if (wait_event_interruptible(alert_wq, alert_ready))
        return -ERESTARTSYS;

    mutex_lock(&alert_lock);
    kbuf = alert_char;
    alert_ready = 0;
    mutex_unlock(&alert_lock);

    if (copy_to_user(buf, &kbuf, 1))
        return -EFAULT;

    return 1;
}

static struct file_operations fops_rx = {
    .owner = THIS_MODULE,
    .read = uart_read,
};

// Module init
static int __init uart_module_init(void) {
    int ret;

    uart_fp = filp_open(UART_DEV, O_RDWR | O_NOCTTY, 0);
    if (IS_ERR(uart_fp)) {
        pr_err("Cannot open UART device %s\n", UART_DEV);
        return PTR_ERR(uart_fp);
    }

    ret = alloc_chrdev_region(&dev_tx, 0, 1, DEVICE_NAME_TX);
    if (ret)
        return ret;
    cdev_init(&cdev_tx, &fops_tx);
    cdev_add(&cdev_tx, dev_tx, 1);

    ret = alloc_chrdev_region(&dev_rx, 0, 1, DEVICE_NAME_RX);
    if (ret)
        return ret;
    cdev_init(&cdev_rx, &fops_rx);
    cdev_add(&cdev_rx, dev_rx, 1);

    uart_class = class_create("uartclass");
    device_create(uart_class, NULL, dev_tx, NULL, DEVICE_NAME_TX);
    device_create(uart_class, NULL, dev_rx, NULL, DEVICE_NAME_RX);

    rx_thread = kthread_run(uart_rx_fn, NULL, "uart_rx_thread");
    pr_info("x86_B_kernel loaded with /dev/%s and /dev/%s\n", DEVICE_NAME_TX, DEVICE_NAME_RX);
    return 0;
}

// Module exit
static void __exit uart_module_exit(void) {
    if (rx_thread)
        kthread_stop(rx_thread);

    device_destroy(uart_class, dev_tx);
    device_destroy(uart_class, dev_rx);
    class_destroy(uart_class);

    unregister_chrdev_region(dev_tx, 1);
    unregister_chrdev_region(dev_rx, 1);

    cdev_del(&cdev_tx);
    cdev_del(&cdev_rx);

    if (uart_fp)
        filp_close(uart_fp, NULL);

    pr_info("x86_B_kernel unloaded\n");
}

module_init(uart_module_init);
module_exit(uart_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI ChatGPT");
MODULE_DESCRIPTION("x86_B kernel UART TX/RX module using ttyUSB0");

