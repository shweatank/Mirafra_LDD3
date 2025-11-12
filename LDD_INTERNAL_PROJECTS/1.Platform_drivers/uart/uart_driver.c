//uart_platform.c
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "uart_platform"
#define UART_REG_SIZE 0x1000   // Size of UART registers

struct uart_platform_dev {
    void __iomem *base;
    struct cdev cdev;
    dev_t devt;
    struct class *cls;
};

static struct uart_platform_dev *uart_dev;

static int uart_open(struct inode *inode, struct file *file)
{
    pr_info("UART device opened\n");
    return 0;
}

static int uart_release(struct inode *inode, struct file *file)
{
    pr_info("UART device closed\n");
    return 0;
}

static ssize_t uart_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    char data = ioread8(uart_dev->base); // read 1 byte from UART
    if (copy_to_user(buf, &data, 1))
        return -EFAULT;
    return 1;
}

static ssize_t uart_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    char data;
    if (copy_from_user(&data, buf, 1))
        return -EFAULT;
    iowrite8(data, uart_dev->base);
    return 1;
}

static struct file_operations uart_fops = {
    .owner = THIS_MODULE,
    .open = uart_open,
    .release = uart_release,
    .read = uart_read,
    .write = uart_write,
};

static int uart_probe(struct platform_device *pdev)
{
    struct resource *res;
    int ret;

    uart_dev = devm_kzalloc(&pdev->dev, sizeof(*uart_dev), GFP_KERNEL);
    if (!uart_dev)
        return -ENOMEM;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    uart_dev->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(uart_dev->base))
        return PTR_ERR(uart_dev->base);

    ret = alloc_chrdev_region(&uart_dev->devt, 0, 1, DRIVER_NAME);
    if (ret)
        return ret;

    cdev_init(&uart_dev->cdev, &uart_fops);
    uart_dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&uart_dev->cdev, uart_dev->devt, 1);
    if (ret)
        return ret;

    uart_dev->cls = class_create(THIS_MODULE, DRIVER_NAME);
    device_create(uart_dev->cls, NULL, uart_dev->devt, NULL, "uart");

    pr_info("UART platform driver probed\n");
    return 0;
}

static int uart_remove(struct platform_device *pdev)
{
    device_destroy(uart_dev->cls, uart_dev->devt);
    class_destroy(uart_dev->cls);
    cdev_del(&uart_dev->cdev);
    unregister_chrdev_region(uart_dev->devt, 1);
    pr_info("UART platform driver removed\n");
    return 0;
}

static const struct of_device_id uart_of_match[] = {
    { .compatible = "custom,uart" },
    {},
};
MODULE_DEVICE_TABLE(of, uart_of_match);

static struct platform_driver uart_platform_driver = {
    .probe = uart_probe,
    .remove = uart_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = uart_of_match,
    },
};

module_platform_driver(uart_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha");
MODULE_DESCRIPTION("UART Platform Driver Example");
