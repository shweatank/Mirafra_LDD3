#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DR   0x00  // Data Register
#define FR   0x18  // Flag Register
#define FR_TXFF (1 << 5) // Transmit FIFO full
#define FR_RXFE (1 << 4) // Receive FIFO empty

static void __iomem *base;

static ssize_t my_uart_write(struct file *file, const char __user *buf,
                             size_t count, loff_t *ppos)
{
    char kbuf[64];
    size_t i;

    if (count > sizeof(kbuf))
        count = sizeof(kbuf);

    if (copy_from_user(kbuf, buf, count))
        return -EFAULT;

    for (i = 0; i < count; i++) {
        // wait until TX FIFO is not full
        while (readl(base + FR) & FR_TXFF)
            cpu_relax();

        writel(kbuf[i], base + DR);
    }
    return count;
}

static ssize_t my_uart_read(struct file *file, char __user *buf,
                            size_t count, loff_t *ppos)
{
    char kbuf[64];
    size_t i = 0;

    while (i < count) {
        // wait until RX FIFO not empty
        if (readl(base + FR) & FR_RXFE)
            break;
        kbuf[i++] = readl(base + DR) & 0xFF;
    }

    if (copy_to_user(buf, kbuf, i))
        return -EFAULT;

    return i;
}

static struct file_operations my_uart_fops = {
    .owner = THIS_MODULE,
    .write = my_uart_write,
    .read  = my_uart_read,
};

static int major;

static int my_uart_probe(struct platform_device *pdev)
{
    struct resource *res;

    pr_info("my_uart: probe called\n");

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(base))
        return PTR_ERR(base);

    major = register_chrdev(0, "my_uart", &my_uart_fops);
    if (major < 0)
        return major;

    pr_info("my_uart: registered char device with major %d\n", major);
    return 0;
}

static void my_uart_remove(struct platform_device *pdev)
{
    unregister_chrdev(major, "my_uart");
    pr_info("my_uart: removed\n");
    //return 0;
}

static const struct of_device_id my_uart_of_match[] = {
    { .compatible = "arm,pl011" },  // Match Pi UART
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_uart_of_match);

static struct platform_driver my_uart_driver = {
    .driver = {
        .name = "my_uart",
        .of_match_table = my_uart_of_match,
    },
    .probe = my_uart_probe,
    .remove = my_uart_remove,
};

module_platform_driver(my_uart_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavan");
MODULE_DESCRIPTION("Simple UART TX/RX driver for Raspberry Pi PL011");

