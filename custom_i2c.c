#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>

#define DEVICE_NAME "myi2c"
#define CLASS_NAME  "i2cchar"

#define BSC_BASE       0xFE804000
#define BSC_SIZE       0x20

#define BSC_C          0x00
#define BSC_S          0x04
#define BSC_DLEN       0x08
#define BSC_A          0x0C
#define BSC_FIFO       0x10

#define BSC_C_I2CEN    (1 << 15)
#define BSC_C_ST       (1 << 7)
#define BSC_C_CLEAR    (1 << 4)
#define BSC_C_READ     (1 << 0)
#define BSC_S_DONE     (1 << 1)
#define BSC_S_ERR      (1 << 8)
#define BSC_S_RXD      (1 << 5)
#define BSC_S_TXD      (1 << 4)

static void __iomem *bsc_base;
static dev_t dev_num;
static struct class *i2c_class;
static struct cdev i2c_cdev;

#define MAX_BUF 32
static uint8_t slave_addr = 0x50; // Default slave address

static int i2c_open(struct inode *inode, struct file *file) {
    return 0;
}

static int i2c_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t i2c_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    uint8_t kbuf[MAX_BUF];
    int i;

    if (len > MAX_BUF) len = MAX_BUF;
    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    writel(slave_addr, bsc_base + BSC_A);
    writel(len, bsc_base + BSC_DLEN);

    for (i = 0; i < len; i++)
        writel(kbuf[i], bsc_base + BSC_FIFO);

    writel(BSC_C_I2CEN | BSC_C_CLEAR | BSC_C_ST, bsc_base + BSC_C);
    while (!(readl(bsc_base + BSC_S) & BSC_S_DONE)) cpu_relax();

    writel(BSC_S_DONE | BSC_S_ERR, bsc_base + BSC_S);
    return len;
}

static ssize_t i2c_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    uint8_t kbuf[MAX_BUF];
    int i;

    if (len > MAX_BUF) len = MAX_BUF;

    writel(slave_addr, bsc_base + BSC_A);
    writel(len, bsc_base + BSC_DLEN);
    writel(BSC_C_I2CEN | BSC_C_CLEAR | BSC_C_ST | BSC_C_READ, bsc_base + BSC_C);
    while (!(readl(bsc_base + BSC_S) & BSC_S_DONE)) cpu_relax();

    for (i = 0; i < len; i++) {
        if (readl(bsc_base + BSC_S) & BSC_S_RXD)
            kbuf[i] = readl(bsc_base + BSC_FIFO) & 0xFF;
        else
            kbuf[i] = 0xFF;
    }

    writel(BSC_S_DONE | BSC_S_ERR, bsc_base + BSC_S);
    if (copy_to_user(buf, kbuf, len))
        return -EFAULT;

    return len;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = i2c_open,
    .release = i2c_release,
    .read    = i2c_read,
    .write   = i2c_write,
};

static int __init i2c_init(void) {
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    cdev_init(&i2c_cdev, &fops);
    cdev_add(&i2c_cdev, dev_num, 1);
    i2c_class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(i2c_class, NULL, dev_num, NULL, DEVICE_NAME);

    bsc_base = ioremap(BSC_BASE, BSC_SIZE);
    if (!bsc_base)
        return -ENOMEM;

    pr_info("i2c_char: loaded /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit i2c_exit(void) {
    iounmap(bsc_base);
    device_destroy(i2c_class, dev_num);
    class_destroy(i2c_class);
    cdev_del(&i2c_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("i2c_char: unloaded\n");
}

module_init(i2c_init);
module_exit(i2c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavan & ChatGPT");
MODULE_DESCRIPTION("Custom I2C Char Driver on BCM2711");

