#include <linux/module.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "spi_sd_loop"
#define CLASS_NAME  "spi_sd"

static struct spi_device *spi_dev;
static dev_t dev_num;
static struct cdev spi_cdev;
static struct class *spi_class;

static ssize_t spi_loop_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    u8 tx[512] = {0xFF};  // send dummy data
    u8 rx[512];
    struct spi_transfer t = {
        .tx_buf = tx,
        .rx_buf = rx,
        .len = sizeof(tx),
    };
    struct spi_message m;
    spi_message_init(&m);
    spi_message_add_tail(&t, &m);

    int ret = spi_sync(spi_dev, &m);
    if (ret)
        return -EIO;

    if (copy_to_user(buf, rx, len))
        return -EFAULT;

    return len;
}

static ssize_t spi_loop_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    u8 tx[512];
    if (copy_from_user(tx, buf, len))
        return -EFAULT;

    struct spi_transfer t = {
        .tx_buf = tx,
        .len = len,
    };
    struct spi_message m;
    spi_message_init(&m);
    spi_message_add_tail(&t, &m);

    int ret = spi_sync(spi_dev, &m);
    return ret ? -EIO : len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = spi_loop_read,
    .write = spi_loop_write,
};

static int spi_sd_probe(struct spi_device *spi)
{
    int ret;

    spi_dev = spi;
    spi->mode = SPI_MODE_0;
    spi->max_speed_hz = 500000;
    spi->bits_per_word = 8;
    ret = spi_setup(spi);
    if (ret)
        return ret;

    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    cdev_init(&spi_cdev, &fops);
    cdev_add(&spi_cdev, dev_num, 1);

    spi_class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(spi_class, NULL, dev_num, NULL, DEVICE_NAME);

    pr_info("SPI loopback device registered\n");
    return 0;
}

static int spi_sd_remove(struct spi_device *spi)
{
    device_destroy(spi_class, dev_num);
    class_destroy(spi_class);
    cdev_del(&spi_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("SPI loopback device removed\n");
    return 0;
}

static struct spi_driver spi_sd_driver = {
    .driver = {
        .name = "spi_sd_loopback",
        .owner = THIS_MODULE,
    },
    .probe = spi_sd_probe,
    .remove = spi_sd_remove,
};

static int __init spi_sd_init(void)
{
    return spi_register_driver(&spi_sd_driver);
}

static void __exit spi_sd_exit(void)
{
    spi_unregister_driver(&spi_sd_driver);
}

module_init(spi_sd_init);
module_exit(spi_sd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RPI SPI");
MODULE_DESCRIPTION("Basic SPI Loopback Driver for microSD Card");

