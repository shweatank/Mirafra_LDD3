/*
 * oled_i2c.c - I2C OLED Display Driver (SSD1306)
 * 
 * This driver supports OLED displays using SSD1306 controller
 * via I2C interface with Device Tree support
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "oled_i2c"
#define DEVICE_NAME "oled"

/* SSD1306 Commands */
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_SEGREMAP            0xA1
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22

struct oled_device {
    struct i2c_client *client;
    struct cdev cdev;
    dev_t devt;
    struct class *class;
    int width;
    int height;
};

static struct oled_device *oled_dev;

/* Write command to OLED */
static int oled_write_cmd(struct i2c_client *client, u8 cmd)
{
    u8 buf[2] = {0x00, cmd};  /* 0x00 = command mode */
    int ret;
    
    ret = i2c_master_send(client, buf, 2);
    if (ret < 0) {
        dev_err(&client->dev, "Failed to write command: %d\n", ret);
        return ret;
    }
    return 0;
}

/* Write data to OLED */
static int oled_write_data(struct i2c_client *client, u8 *data, size_t len)
{
    u8 *buf;
    int ret;
    
    buf = kmalloc(len + 1, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;
    
    buf[0] = 0x40;  /* 0x40 = data mode */
    memcpy(buf + 1, data, len);
    
    ret = i2c_master_send(client, buf, len + 1);
    kfree(buf);
    
    if (ret < 0) {
        dev_err(&client->dev, "Failed to write data: %d\n", ret);
        return ret;
    }
    return 0;
}

/* Initialize OLED display */
static int oled_init_display(struct i2c_client *client)
{
    /* Turn off display */
    oled_write_cmd(client, SSD1306_DISPLAYOFF);
    
    /* Set display clock divide ratio */
    oled_write_cmd(client, SSD1306_SETDISPLAYCLOCKDIV);
    oled_write_cmd(client, 0x80);
    
    /* Set multiplex ratio */
    oled_write_cmd(client, SSD1306_SETMULTIPLEX);
    oled_write_cmd(client, 0x3F);  /* 64MUX */
    
    /* Set display offset */
    oled_write_cmd(client, SSD1306_SETDISPLAYOFFSET);
    oled_write_cmd(client, 0x00);
    
    /* Set start line */
    oled_write_cmd(client, SSD1306_SETSTARTLINE | 0x00);
    
    /* Enable charge pump */
    oled_write_cmd(client, SSD1306_CHARGEPUMP);
    oled_write_cmd(client, 0x14);
    
    /* Set memory addressing mode */
    oled_write_cmd(client, SSD1306_MEMORYMODE);
    oled_write_cmd(client, 0x00);  /* Horizontal addressing mode */
    
    /* Set segment remap */
    oled_write_cmd(client, SSD1306_SEGREMAP);
    
    /* Set COM output scan direction */
    oled_write_cmd(client, SSD1306_COMSCANDEC);
    
    /* Set COM pins hardware configuration */
    oled_write_cmd(client, SSD1306_SETCOMPINS);
    oled_write_cmd(client, 0x12);
    
    /* Set contrast */
    oled_write_cmd(client, SSD1306_SETCONTRAST);
    oled_write_cmd(client, 0xCF);
    
    /* Set pre-charge period */
    oled_write_cmd(client, SSD1306_SETPRECHARGE);
    oled_write_cmd(client, 0xF1);
    
    /* Set VCOMH deselect level */
    oled_write_cmd(client, SSD1306_SETVCOMDETECT);
    oled_write_cmd(client, 0x40);
    
    /* Entire display on (resume) */
    oled_write_cmd(client, SSD1306_DISPLAYALLON_RESUME);
    
    /* Set normal display */
    oled_write_cmd(client, SSD1306_NORMALDISPLAY);
    
    /* Turn on display */
    oled_write_cmd(client, SSD1306_DISPLAYON);
    
    dev_info(&client->dev, "OLED display initialized\n");
    return 0;
}

/* Clear display */
static int oled_clear_display(struct i2c_client *client)
{
    u8 clear_buf[128] = {0};
    int page;
    
    /* Set column address range */
    oled_write_cmd(client, SSD1306_COLUMNADDR);
    oled_write_cmd(client, 0);    /* Start column */
    oled_write_cmd(client, 127);  /* End column */
    
    /* Set page address range */
    oled_write_cmd(client, SSD1306_PAGEADDR);
    oled_write_cmd(client, 0);    /* Start page */
    oled_write_cmd(client, 7);    /* End page */
    
    /* Clear all pages */
    for (page = 0; page < 8; page++) {
        oled_write_data(client, clear_buf, 128);
    }
    
    return 0;
}

/* File operations */
static int oled_open(struct inode *inode, struct file *file)
{
    file->private_data = oled_dev;
    return 0;
}

static int oled_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t oled_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *ppos)
{
    struct oled_device *dev = file->private_data;
    u8 *kernel_buf;
    int ret;
    
    if (count > 1024)
        count = 1024;
    
    kernel_buf = kmalloc(count, GFP_KERNEL);
    if (!kernel_buf)
        return -ENOMEM;
    
    if (copy_from_user(kernel_buf, buf, count)) {
        kfree(kernel_buf);
        return -EFAULT;
    }
    
    ret = oled_write_data(dev->client, kernel_buf, count);
    kfree(kernel_buf);
    
    return (ret < 0) ? ret : count;
}

static const struct file_operations oled_fops = {
    .owner = THIS_MODULE,
    .open = oled_open,
    .release = oled_release,
    .write = oled_write,
};

/* I2C probe function */
static int oled_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct device_node *np = client->dev.of_node;
    int ret;
    
    dev_info(&client->dev, "OLED driver probing\n");
    
    /* Allocate device structure */
    oled_dev = devm_kzalloc(&client->dev, sizeof(*oled_dev), GFP_KERNEL);
    if (!oled_dev)
        return -ENOMEM;
    
    oled_dev->client = client;
    i2c_set_clientdata(client, oled_dev);
    
    /* Read device tree properties */
    if (np) {
        of_property_read_u32(np, "width", &oled_dev->width);
        of_property_read_u32(np, "height", &oled_dev->height);
    }
    
    /* Default to 128x64 if not specified */
    if (oled_dev->width == 0)
        oled_dev->width = 128;
    if (oled_dev->height == 0)
        oled_dev->height = 64;
    
    /* Initialize display */
    ret = oled_init_display(client);
    if (ret < 0) {
        dev_err(&client->dev, "Failed to initialize display\n");
        return ret;
    }
    
    /* Clear display */
    oled_clear_display(client);
    
    /* Create character device */
    ret = alloc_chrdev_region(&oled_dev->devt, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        dev_err(&client->dev, "Failed to allocate char device region\n");
        return ret;
    }
    
    cdev_init(&oled_dev->cdev, &oled_fops);
    oled_dev->cdev.owner = THIS_MODULE;
    
    ret = cdev_add(&oled_dev->cdev, oled_dev->devt, 1);
    if (ret < 0) {
        unregister_chrdev_region(oled_dev->devt, 1);
        return ret;
    }
    
    /* Create device class */
    oled_dev->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(oled_dev->class)) {
        cdev_del(&oled_dev->cdev);
        unregister_chrdev_region(oled_dev->devt, 1);
        return PTR_ERR(oled_dev->class);
    }
    
    device_create(oled_dev->class, NULL, oled_dev->devt, NULL, DEVICE_NAME);
    
    dev_info(&client->dev, "OLED driver probed successfully (Resolution: %dx%d)\n",
             oled_dev->width, oled_dev->height);
    
    return 0;
}

/* I2C remove function */
static int oled_remove(struct i2c_client *client)
{
    struct oled_device *dev = i2c_get_clientdata(client);
    
    /* Clear display before removing */
    oled_clear_display(client);
    
    /* Remove device */
    device_destroy(dev->class, dev->devt);
    class_destroy(dev->class);
    cdev_del(&dev->cdev);
    unregister_chrdev_region(dev->devt, 1);
    
    dev_info(&client->dev, "OLED driver removed\n");
    return 0;
}

/* Device tree match table */
static const struct of_device_id oled_of_match[] = {
    { .compatible = "solomon,ssd1306" },
    { }
};
MODULE_DEVICE_TABLE(of, oled_of_match);

/* I2C device ID table */
static const struct i2c_device_id oled_id[] = {
    { "ssd1306", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, oled_id);

/* I2C driver structure */
static struct i2c_driver oled_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = oled_of_match,
    },
    .probe = oled_probe,
    .remove = oled_remove,
    .id_table = oled_id,
};

module_i2c_driver(oled_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MK");
MODULE_DESCRIPTION("I2C OLED Display Driver ");
MODULE_VERSION("1.0");
