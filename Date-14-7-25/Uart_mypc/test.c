// SPDX‑License‑Identifier: GPL‑2.0
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DRIVER      "pl011_loop"
#define UART_BASE   0xFE201000      /* PL011 UART0 on Pi‑4 (after enable_uart=1) */
#define UART_DR     0x00            /* Data register              */
#define UART_FR     0x18            /* Flag register              */
#define FR_TXFF     0x20            /* Transmit FIFO full         */
#define FR_RXFE     0x10            /* Receive FIFO empty         */

static dev_t dev;
static struct cdev cdev;
static struct class *cls;
static void __iomem *uart;

static ssize_t loop_write(struct file *f,const char __user *ub,size_t n,loff_t*o)
{
    unsigned char ch;
    if (n < 1) return 0;
    if (copy_from_user(&ch, ub, 1)) return -EFAULT;

    while (readl(uart + UART_FR) & FR_TXFF) cpu_relax();
    writel(ch, uart + UART_DR);
    return 1;
}

static ssize_t loop_read(struct file *f,char __user *ub,size_t n,loff_t*o)
{
    unsigned char ch;
    int t = 100000;                               /* ≈ few ms busy‑wait */
    while ((readl(uart + UART_FR) & FR_RXFE) && t--) cpu_relax();
    if (t<=0) return 0;                           /* nothing received */
    ch = readl(uart + UART_DR) & 0xFF;
    return copy_to_user(ub, &ch, 1) ? -EFAULT : 1;
}

static const struct file_operations fops = {
    .owner  = THIS_MODULE,
    .write  = loop_write,
    .read   = loop_read,
};

static int __init loop_init(void)
{
    if (alloc_chrdev_region(&dev,0,1,DRIVER)) return -EBUSY;
    cdev_init(&cdev,&fops);
    if (cdev_add(&cdev,dev,1)) goto err_chr;
    cls = class_create(THIS_MODULE,DRIVER);
    if (IS_ERR(cls)) goto err_cdev;
    device_create(cls,NULL,dev,NULL,DRIVER);

    uart = ioremap(UART_BASE,0x48);
    if (!uart) goto err_dev;

    pr_info(DRIVER ": loaded – /dev/%s (PL011 loop‑back)\n", DRIVER);
    return 0;
err_dev: device_destroy(cls,dev);
err_cdev: class_destroy(cls);
err_chr:  unregister_chrdev_region(dev,1);
    return -ENOMEM;
}

static void __exit loop_exit(void)
{
    iounmap(uart);
    device_destroy(cls,dev);
    class_destroy(cls);
    cdev_del(&cdev);
    unregister_chrdev_region(dev,1);
    pr_info(DRIVER ": unloaded\n");
}

module_init(loop_init);
module_exit(loop_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PL011 UART loop‑back driver for Raspberry Pi 4");

