/*
 * ssd1306_fb_6_12.c - SSD1306 I2C framebuffer driver for Linux 6.12 (RPi)
 * Debug + auto-reset + compatible with 128x32 OLED
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

struct ssd1306_dev {
    struct i2c_client *client;
    struct fb_info *info;
    struct gpio_desc *reset_gpio;
    u8 *fb_data;
    int width;
    int height;
};

static void ssd1306_reset(struct ssd1306_dev *dev)
{
    if (!dev->reset_gpio) return;
    gpiod_set_value(dev->reset_gpio, 0);
    mdelay(10);
    gpiod_set_value(dev->reset_gpio, 1);
    mdelay(10);
    pr_info("ssd1306: reset done\n");
}

static void ssd1306_update(struct ssd1306_dev *dev)
{
    int pages = dev->height / 8;
    int i;
    pr_info("ssd1306_update called\n");

    for (i = 0; i < pages; i++) {
        // send page command (simplified)
        i2c_smbus_write_byte_data(dev->client, 0xB0 + i, 0x00); // page select
        i2c_smbus_write_byte_data(dev->client, 0x00, 0x10); // column start
        i2c_smbus_write_i2c_block_data(dev->client, 0x40, dev->width, dev->fb_data + i*dev->width);
    }
}

static ssize_t ssd1306_fb_write(struct fb_info *info, const char __user *buf, size_t count, loff_t *ppos)
{
    struct ssd1306_dev *dev = info->par;
    size_t bytes = min(count, (size_t)(dev->width*dev->height/8));

    if (copy_from_user(dev->fb_data, buf, bytes))
        return -EFAULT;

    ssd1306_update(dev);
    return bytes;
}

static int ssd1306_probe(struct i2c_client *client)
{
    struct ssd1306_dev *dev;
    struct fb_info *info;
    int fb_size;

    dev = devm_kzalloc(&client->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev) return -ENOMEM;

    dev->client = client;
    dev->width = 128;
    dev->height = 32;
    dev->reset_gpio = devm_gpiod_get_optional(&client->dev, "reset", GPIOD_OUT_HIGH);
    ssd1306_reset(dev);

    fb_size = dev->width * dev->height / 8;
    dev->fb_data = devm_kzalloc(&client->dev, fb_size, GFP_KERNEL);
    if (!dev->fb_data) return -ENOMEM;

    info = framebuffer_alloc(0, &client->dev);
    if (!info) return -ENOMEM;

    static struct fb_ops ssd1306_fbops = {
        .owner = THIS_MODULE,
        .fb_write = ssd1306_fb_write,
    };
    info->par = dev;
    info->fbops = &ssd1306_fbops;
    info->screen_base = (char __iomem *)dev->fb_data;
    info->fix.smem_len = fb_size;
    info->var.xres = dev->width;
    info->var.yres = dev->height;
    info->fix.line_length = dev->width;

    register_framebuffer(info);
    dev->info = info;

    i2c_set_clientdata(client, dev);
    pr_info("ssd1306_fb_6_12: framebuffer registered %dx%d\n", dev->width, dev->height);

    // initial clear: all pixels on for debug
    memset(dev->fb_data, 0xFF, fb_size);
    ssd1306_update(dev);

    return 0;
}

static void ssd1306_remove(struct i2c_client *client)
{
    struct ssd1306_dev *dev = i2c_get_clientdata(client);
    //if (!dev) return 0;
    unregister_framebuffer(dev->info);
    devm_kfree(&client->dev, dev->fb_data);
}

static const struct i2c_device_id ssd1306_id[] = {
    { "ssd1306_fb_debug", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, ssd1306_id);

static const struct of_device_id ssd1306_of_match[] = {
    { .compatible = "pavan,ssd1306-fb" },
    { }
};
MODULE_DEVICE_TABLE(of, ssd1306_of_match);

static struct i2c_driver ssd1306_driver = {
    .driver = {
        .name = "ssd1306_fb_debug",
        .of_match_table = ssd1306_of_match,
    },
    .probe = ssd1306_probe,
    .remove = ssd1306_remove,
    .id_table = ssd1306_id,
};
module_i2c_driver(ssd1306_driver);

MODULE_AUTHOR("Pavan");
MODULE_DESCRIPTION("SSD1306 I2C framebuffer for RPi Linux 6.12 with debug and auto-reset");
MODULE_LICENSE("GPL");

