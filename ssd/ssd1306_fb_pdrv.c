// ssd1306_fb_pdrv.c
// Minimal, self-contained I²C framebuffer driver for SSD1306
// - Works on Linux 6.x (uses .probe_new and void .remove)
// - Binds ONLY to a custom compatible string to avoid conflicts with upstream DRM ssd130x
// - Supports 128x64 and 128x32 panels (set via DT)
// - Optional reset GPIO (reset-gpios)
// - Optional rotation (0 or 180) via DT property "rotation"
//
// Build: see provided Makefile (out-of-tree module)
// DTS overlay: see ssd1306-overlay.dts in this canvas

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DRV_NAME "ssd1306_fb_pdrv"

struct ssd1306 {
    struct i2c_client *client;
    struct fb_info *info;
    struct gpio_desc *reset_gpiod; // optional

    unsigned int width;   // pixels (e.g., 128)
    unsigned int height;  // pixels (e.g., 64 or 32)
    unsigned int pages;   // height / 8
    size_t buf_size;      // width * height / 8

    u8 *buffer;           // shadow framebuffer in RAM
    u8 *txbuf;            // 1 + width (control byte + data) for page uploads

    u32 rotation;         // 0 or 180
};

/* ---------------- I2C helpers ---------------- */
static int ssd1306_write_cmd(struct i2c_client *client, u8 cmd)
{
    u8 tmp[2] = { 0x00, cmd }; // 0x00 indicates command
    int ret = i2c_master_send(client, tmp, 2);
    return (ret == 2) ? 0 : (ret < 0 ? ret : -EIO);
}

static int ssd1306_write_cmd2(struct i2c_client *client, u8 c1, u8 c2)
{
    u8 tmp[3] = { 0x00, c1, c2 };
    int ret = i2c_master_send(client, tmp, 3);
    return (ret == 3) ? 0 : (ret < 0 ? ret : -EIO);
}

static int ssd1306_write_data(struct i2c_client *client, const u8 *data, size_t len)
{
    // First byte 0x40 indicates data stream
    struct ssd1306 *ctx = i2c_get_clientdata(client);
    if (len > ctx->width)
        len = ctx->width;

    ctx->txbuf[0] = 0x40;
    memcpy(&ctx->txbuf[1], data, len);

    {
        int ret = i2c_master_send(client, ctx->txbuf, len + 1);
        return (ret == (int)(len + 1)) ? 0 : (ret < 0 ? ret : -EIO);
    }
}

/* ---------------- Panel update ---------------- */
static int ssd1306_update(struct ssd1306 *ctx)
{
    unsigned int page;
    int ret;

    for (page = 0; page < ctx->pages; page++) {
        const u8 *src = ctx->buffer + (page * ctx->width);

        // Set page address (0xB0..)
        ret = ssd1306_write_cmd(ctx->client, 0xB0 + page);
        if (ret)
            return ret;

        // Set column low/high nibbles to 0
        ret = ssd1306_write_cmd(ctx->client, 0x00); // lower column addr
        if (ret)
            return ret;
        ret = ssd1306_write_cmd(ctx->client, 0x10); // higher column addr
        if (ret)
            return ret;

        ret = ssd1306_write_data(ctx->client, src, ctx->width);
        if (ret)
            return ret;
    }

    return 0;
}

/* ---------------- fbops wrappers ---------------- */
static void ssd1306_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
    sys_fillrect(info, rect);
    ssd1306_update(info->par);
}

static void ssd1306_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
    sys_copyarea(info, area);
    ssd1306_update(info->par);
}

static void ssd1306_imageblit(struct fb_info *info, const struct fb_image *image)
{
    sys_imageblit(info, image);
    ssd1306_update(info->par);
}

static ssize_t ssd1306_write(struct fb_info *info, const char __user *buf,
                             size_t count, loff_t *ppos)
{
    struct ssd1306 *ctx = info->par;
    size_t max = ctx->buf_size;

    if (*ppos >= max)
        return 0;
    if (count > max - *ppos)
        count = max - *ppos;

    if (copy_from_user(ctx->buffer + *ppos, buf, count))
        return -EFAULT;

    *ppos += count;

    ssd1306_update(ctx);
    return count;
}

static int ssd1306_blank(int blank_mode, struct fb_info *info)
{
    struct ssd1306 *ctx = info->par;
    int ret;

    if (blank_mode == FB_BLANK_UNBLANK) {
        // Display ON
        ret = ssd1306_write_cmd(ctx->client, 0xAF);
    } else {
        // Display OFF
        ret = ssd1306_write_cmd(ctx->client, 0xAE);
    }
    return ret;
}

static struct fb_ops ssd1306_fbops = {
    .owner        = THIS_MODULE,
    .fb_read      = fb_sys_read,
    .fb_write     = ssd1306_write,
    .fb_fillrect  = ssd1306_fillrect,
    .fb_copyarea  = ssd1306_copyarea,
    .fb_imageblit = ssd1306_imageblit,
    .fb_blank     = ssd1306_blank,
};

/* ---------------- Panel init ---------------- */
static int ssd1306_hw_reset(struct ssd1306 *ctx)
{
    if (!ctx->reset_gpiod)
        return 0;

    gpiod_set_value_cansleep(ctx->reset_gpiod, 0);
    msleep(1);
    gpiod_set_value_cansleep(ctx->reset_gpiod, 1);
    msleep(10);
    return 0;
}

static int ssd1306_init(struct ssd1306 *ctx)
{
    int ret;
    u8 com_pins;
    u8 mux = (ctx->height == 32) ? 0x1F : 0x3F; // 32->0x1F, 64->0x3F

    com_pins = (ctx->height == 32) ? 0x02 : 0x12; // values commonly used

    ret = ssd1306_hw_reset(ctx);
    if (ret)
        return ret;

    // Init sequence (based on SSD1306 datasheet)
    ret = ssd1306_write_cmd(ctx->client, 0xAE); // Display off
    if (ret) return ret;

    ret = ssd1306_write_cmd(ctx->client, 0xD5); // Set display clock divide
    if (ret) return ret;
    ret = ssd1306_write_cmd(ctx->client, 0x80); // suggested ratio
    if (ret) return ret;

    ret = ssd1306_write_cmd(ctx->client, 0xA8); // Set multiplex
    if (ret) return ret;
    ret = ssd1306_write_cmd(ctx->client, mux);
    if (ret) return ret;

    ret = ssd1306_write_cmd2(ctx->client, 0xD3, 0x00); // Display offset
    if (ret) return ret;

    ret = ssd1306_write_cmd(ctx->client, 0x40); // start line 0
    if (ret) return ret;

    // rotation handling: segment remap and COM scan direction
    if (ctx->rotation == 180) {
        ret = ssd1306_write_cmd(ctx->client, 0xA0); // SEG remap normal
        if (ret) return ret;
        ret = ssd1306_write_cmd(ctx->client, 0xC0); // COM scan inc
        if (ret) return ret;
    } else {
        ret = ssd1306_write_cmd(ctx->client, 0xA1); // SEG remap mirror
        if (ret) return ret;
        ret = ssd1306_write_cmd(ctx->client, 0xC8); // COM scan dec
        if (ret) return ret;
    }

    ret = ssd1306_write_cmd2(ctx->client, 0xDA, com_pins); // COM pins
    if (ret) return ret;

    ret = ssd1306_write_cmd2(ctx->client, 0x81, 0x7F); // contrast
    if (ret) return ret;

    ret = ssd1306_write_cmd(ctx->client, 0xA4); // display from RAM
    if (ret) return ret;

    ret = ssd1306_write_cmd(ctx->client, 0xA6); // normal (not inverted)
    if (ret) return ret;

    ret = ssd1306_write_cmd2(ctx->client, 0x20, 0x00); // memory mode: horizontal
    if (ret) return ret;

    ret = ssd1306_write_cmd(ctx->client, 0xAF); // Display ON
    if (ret) return ret;

    // Clear display once
    memset(ctx->buffer, 0x00, ctx->buf_size);
    return ssd1306_update(ctx);
}

/* ---------------- I2C probe/remove ---------------- */
static int ssd1306_probe(struct i2c_client *client)
{
    struct device *dev = &client->dev;
    struct fb_info *info;
    struct ssd1306 *ctx;
    u32 width = 128, height = 64, rotation = 0;
    int ret;

    // Read DT properties (width/height/rotation)
    of_property_read_u32(dev->of_node, "width", &width);
    of_property_read_u32(dev->of_node, "height", &height);
    of_property_read_u32(dev->of_node, "rotation", &rotation);

    if ((height != 32 && height != 64) || width != 128) {
        dev_warn(dev, "Unsupported size %ux%u; defaulting to 128x64\n", width, height);
        width = 128; height = 64;
    }
    if (rotation != 0 && rotation != 180) {
        dev_warn(dev, "Unsupported rotation %u; using 0\n", rotation);
        rotation = 0;
    }

    info = framebuffer_alloc(sizeof(*ctx), dev);
    if (!info)
        return -ENOMEM;

    ctx = info->par;
    ctx->client = client;
    ctx->info = info;
    ctx->width = width;
    ctx->height = height;
    ctx->pages = height / 8;
    ctx->buf_size = (size_t)width * height / 8;
    ctx->rotation = rotation;

    ctx->buffer = devm_kzalloc(dev, ctx->buf_size, GFP_KERNEL);
    if (!ctx->buffer) { ret = -ENOMEM; goto err_rel; }

    ctx->txbuf = devm_kzalloc(dev, width + 1, GFP_KERNEL);
    if (!ctx->txbuf) { ret = -ENOMEM; goto err_rel; }

    // Optional reset GPIO
    ctx->reset_gpiod = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->reset_gpiod)) {
        ret = PTR_ERR(ctx->reset_gpiod);
        dev_err(dev, "reset-gpios error: %d\n", ret);
        goto err_rel;
    }

    // fb_fix
    strscpy(info->fix.id, DRV_NAME, sizeof(info->fix.id));
    info->fix.smem_len   = ctx->buf_size;
    info->fix.type       = FB_TYPE_PACKED_PIXELS;
    info->fix.visual     = FB_VISUAL_MONO01;
    info->fix.line_length= ctx->width / 8; // bytes per line for 1bpp

    // fb_var
    info->var.xres           = ctx->width;
    info->var.yres           = ctx->height;
    info->var.xres_virtual   = ctx->width;
    info->var.yres_virtual   = ctx->height;
    info->var.bits_per_pixel = 1;
    info->var.grayscale      = 1;

    info->screen_base = (char __iomem *)ctx->buffer;
    info->fbops = &ssd1306_fbops;

    i2c_set_clientdata(client, ctx);

    ret = ssd1306_init(ctx);
    if (ret) {
        dev_err(dev, "init failed: %d\n", ret);
        goto err_rel;
    }

    ret = register_framebuffer(info);
    if (ret) {
        dev_err(dev, "register_framebuffer failed: %d\n", ret);
        goto err_rel;
    }

    dev_info(dev, "SSD1306 %ux%u framebuffer registered as %s (rotation=%u)\n",
             ctx->width, ctx->height, info->fix.id, ctx->rotation);
    return 0;

err_rel:
    framebuffer_release(info);
    return ret;
}

static void ssd1306_remove(struct i2c_client *client)
{
    struct ssd1306 *ctx = i2c_get_clientdata(client);

    if (!ctx || !ctx->info)
        return;

    unregister_framebuffer(ctx->info);
    framebuffer_release(ctx->info);
}

/* ---------------- OF/ID tables + driver ---------------- */
static const struct of_device_id ssd1306_of_match[] = {
    { .compatible = "pavan,ssd1306-fb" }, // custom string to avoid conflicts
    {}
};
MODULE_DEVICE_TABLE(of, ssd1306_of_match);

static const struct i2c_device_id ssd1306_ids[] = {
    { "ssd1306-fb", 0 },
    {}
};
MODULE_DEVICE_TABLE(i2c, ssd1306_ids);

static struct i2c_driver ssd1306_driver = {
    .driver = {
        .name = DRV_NAME,
        .of_match_table = ssd1306_of_match,
    },
    .probe = ssd1306_probe, // Linux 6.x API
    .remove    = ssd1306_remove, // void remove in 6.x
    .id_table  = ssd1306_ids,
};

module_i2c_driver(ssd1306_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You <you@example.com>");
MODULE_DESCRIPTION("SSD1306 I2C framebuffer (custom platform-like driver)");

