#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DR      0x00   // Data Register
#define FR      0x18   // Flag Register
#define FR_TXFF (1 << 5) // Transmit FIFO full
#define FR_RXFE (1 << 4) // Receive FIFO empty

static void __iomem *uart_base;

static void uart_send_char(char c)
{
    // Wait until TX FIFO not full
    while (readl(uart_base + FR) & FR_TXFF)
        cpu_relax();
    writel(c, uart_base + DR);
}

static char uart_read_char(void)
{
    // Wait until RX FIFO not empty
    while (readl(uart_base + FR) & FR_RXFE)
        cpu_relax();
    return (char)readl(uart_base + DR);
}

static int uart_probe(struct platform_device *pdev)
{
    struct resource *res;

    pr_info("UART: probe called\n");

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res)
        return -ENODEV;

    uart_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(uart_base))
        return PTR_ERR(uart_base);

    // Example usage
    uart_send_char('H');
    uart_send_char('i');
    uart_send_char('\n');

    pr_info("UART sent chars, now reading back...\n");
    pr_info("Received: %c\n", uart_read_char());

    return 0;
}

static void uart_remove(struct platform_device *pdev)
{
    pr_info("UART: remove called\n");
    return ;
}

static const struct of_device_id uart_of_match[] = {
    { .compatible = "custom,uart_demo" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, uart_of_match);

static struct platform_driver uart_platform_driver = {
    .probe  = uart_probe,
    .remove = uart_remove,
    .driver = {
        .name = "my_uart_driver",
        .of_match_table = uart_of_match,
    },
};

module_platform_driver(uart_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavan");
MODULE_DESCRIPTION("UART platform driver");
