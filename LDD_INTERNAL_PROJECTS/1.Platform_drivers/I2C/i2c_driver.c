// SPDX-License-Identifier: GPL-2.0
/*
 * SSD1306 OLED Display Platform Driver (via I2C)
 * Author: Harsha Sri Sampath
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/string.h>

#define DEVICE_NAME "ssd1306"
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32
#define SSD1306_BUF_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

struct ssd1306_dev {
    struct i2c_client *client;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    u8 buffer[SSD1306_BUF_SIZE];
};

/* --- I2C Command Utilities --- */
static int ssd1306_send_cmd(struct i2c_client *client, u8 cmd)
{
    u8 buf[2] = { 0x80, cmd };  // 0x80 = control byte for command
    return i2c_master_send(client, buf, 2);
}

/* --- OLED Initialization --- */
static int ssd1306_init_display(struct i2c_client *client)
{
    int ret;
    const u8 init_seq[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x1F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x02,
        0x81, 0x8F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6,
        0x2E, 0xAF
    };
    for (int i = 0; i < ARRAY_SIZE(init_seq); i++) {
        ret = ssd1306_send_cmd(client, init_seq[i]);
        if (ret < 0)
            return ret;
    }
    return 0;
}

/* --- Character Device Operations --- */
static int ssd1306_open(struct inode *inode, struct file *filp)
{
    struct ssd1306_dev *dev = container_of(inode->i_cdev, struct ssd1306_dev, cdev);
    filp->private_data = dev;
    return 0;
}

static ssize_t ssd1306_write(struct file *file, const char __user *buf,
                             size_t count, loff_t *ppos)
{
    struct ssd1306_dev *dev = file->private_data;
    u8 i2c_buf[1 + SSD1306_WIDTH];
    int page, ret;

    if (count < SSD1306_BUF_SIZE)
        return -EINVAL;

    if (copy_from_user(dev->buffer, buf, SSD1306_BUF_SIZE))
        return -EFAULT;

    for (page = 0; page < SSD1306_HEIGHT / 8; page++) {
        ssd1306_send_cmd(dev->client, 0xB0 + page);
        ssd1306_send_cmd(dev->client, 0x00);
        ssd1306_send_cmd(dev->client, 0x10);

        i2c_buf[0] = 0x40; // data control byte
        memcpy(&i2c_buf[1], &dev->buffer[page * SSD1306_WIDTH], SSD1306_WIDTH);
        ret = i2c_master_send(dev->client, i2c_buf, SSD1306_WIDTH + 1);
        if (ret < 0)
            return ret;
    }
    return SSD1306_BUF_SIZE;
}

static int ssd1306_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static const struct file_operations ssd1306_fops = {
    .owner = THIS_MODULE,
    .open = ssd1306_open,
    .write = ssd1306_write,
    .release = ssd1306_release,
};

/* --- Platform Probe --- */
static int ssd1306_probe(struct platform_device *pdev)
{
    struct ssd1306_dev *dev;
    struct i2c_adapter *adapter;
    struct i2c_board_info info = { };
    struct i2c_client *client;
    dev_t devno;
    static int major;
    int ret;
    u32 i2c_bus, i2c_addr;

    pr_info("SSD1306 platform probe called\n");

    /* Get I2C bus number and address from DTS */
    if (of_property_read_u32(pdev->dev.of_node, "i2c-bus", &i2c_bus) ||
        of_property_read_u32(pdev->dev.of_node, "i2c-addr", &i2c_addr)) {
        dev_err(&pdev->dev, "Missing I2C parameters in DT\n");
        return -EINVAL;
    }

    adapter = i2c_get_adapter(i2c_bus);
    if (!adapter)
        return -ENODEV;

    strscpy(info.type, "ssd1306", I2C_NAME_SIZE);
    info.addr = i2c_addr;

    client = i2c_new_client_device(adapter, &info);
    i2c_put_adapter(adapter);
    if (IS_ERR(client))
        return PTR_ERR(client);

    /* Allocate and register device structure */
    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->client = client;
    platform_set_drvdata(pdev, dev);

    /* Char device registration */
    if (!major) {
        ret = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
        if (ret < 0)
            return ret;
        major = MAJOR(devno);
    } else {
        devno = MKDEV(major, 0);
    }

    cdev_init(&dev->cdev, &ssd1306_fops);
    ret = cdev_add(&dev->cdev, devno, 1);
    if (ret)
        return ret;

    dev->class = class_create(DEVICE_NAME);
    if (IS_ERR(dev->class))
        return PTR_ERR(dev->class);

    dev->device = device_create(dev->class, NULL, devno, NULL, DEVICE_NAME);
    if (IS_ERR(dev->device))
        return PTR_ERR(dev->device);

    /* Initialize OLED */
    ret = ssd1306_init_display(client);
    if (ret < 0)
        dev_err(&pdev->dev, "Failed to init OLED\n");
    else
        pr_info("SSD1306 OLED initialized successfully\n");

    return 0;
}

/* --- Platform Remove --- */
static int ssd1306_remove(struct platform_device *pdev)
{
    struct ssd1306_dev *dev = platform_get_drvdata(pdev);
    dev_t devno = dev->cdev.dev;

    device_destroy(dev->class, devno);
    class_destroy(dev->class);
    cdev_del(&dev->cdev);
    unregister_chrdev_region(devno, 1);

    i2c_unregister_device(dev->client);
    pr_info("SSD1306 platform driver removed\n");
    return 0;
}

/* --- Device Tree Match Table --- */
static const struct of_device_id ssd1306_of_match[] = {
    { .compatible = "my_platform,ssd1306" },
    { }
};
MODULE_DEVICE_TABLE(of, ssd1306_of_match);

/* --- Platform Driver Structure --- */
static struct platform_driver ssd1306_platform_driver = {
    .probe  = ssd1306_probe,
    .remove = ssd1306_remove,
    .driver = {
        .name = DEVICE_NAME,
        .of_match_table = ssd1306_of_match,
    },
};

module_platform_driver(ssd1306_platform_driver);

MODULE_AUTHOR("Harsha Sri Sampath");
MODULE_DESCRIPTION("SSD1306 Platform Driver (with internal I2C client)");
MODULE_LICENSE("GPL");

