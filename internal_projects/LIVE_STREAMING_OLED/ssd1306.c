// ssd1306_fb.c
// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/fb.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/pm.h>
#include <video/cfb_fillrect.h>
#include <video/cfb_copyarea.h>
#include <video/cfb_imageblit.h>

#define DRV_NAME "ssd1306_fb"
#define CTRL_CMD  0x00
#define CTRL_DATA 0x40

struct ssd1306 {
	struct i2c_client *client;
	struct gpio_desc *reset;
	struct regulator *vbat;
	struct fb_info *info;
	u8 *vram;          /* shadow VRAM (1 bpp, packed vertical-bytes = height/8 per column) */
	int width;         /* pixels */
	int height;        /* pixels (must be multiple of 8) */
	int pages;         /* height / 8 */
	struct mutex lock;
	bool powered;
	u8 contrast;
	u8 col_offset;     /* for SH1106 panels (2..4), default 0 */
};

static int oled_send_cmd(struct ssd1306 *s, u8 c)
{
	u8 buf[2] = { CTRL_CMD, c };
	int ret = i2c_master_send(s->client, buf, 2);
	return (ret == 2) ? 0 : (ret < 0 ? ret : -EIO);
}

static int oled_send_cmd2(struct ssd1306 *s, u8 c1, u8 c2)
{
	u8 buf[3] = { CTRL_CMD, c1, c2 };
	int ret = i2c_master_send(s->client, buf, 3);
	return (ret == 3) ? 0 : (ret < 0 ? ret : -EIO);
}

static int oled_send_data(struct ssd1306 *s, const u8 *data, size_t len)
{
	/* Send in chunks, prepend 0x40 control byte */
	u8 *tmp;
	size_t off = 0;
	int ret = 0;

	if (!len) return 0;
	tmp = kmalloc(len + 1, GFP_KERNEL);
	if (!tmp) return -ENOMEM;

	while (off < len) {
		size_t chunk = min_t(size_t, len - off, 1024);
		tmp[0] = CTRL_DATA;
		memcpy(tmp + 1, data + off, chunk);
		ret = i2c_master_send(s->client, tmp, chunk + 1);
		if (ret != chunk + 1) {
			ret = (ret < 0) ? ret : -EIO;
			break;
		}
		off += chunk;
	}
	kfree(tmp);
	return ret ? ret : 0;
}

static int oled_set_page_col(struct ssd1306 *s, int page, int col)
{
	col += s->col_offset; /* SH1106 column offset if any */
	return
		oled_send_cmd(s, 0xB0 | (page & 0x0F)) ?:               /* page addr */
		oled_send_cmd(s, 0x00 | (col & 0x0F)) ?:                /* lower col */
		oled_send_cmd(s, 0x10 | ((col >> 4) & 0x0F));           /* higher col */
}

static int oled_update_all(struct ssd1306 *s)
{
	int p, ret = 0;
	for (p = 0; p < s->pages; p++) {
		ret = oled_set_page_col(s, p, 0);
		if (ret) return ret;
		/* Our VRAM is arranged page-major contiguous: width bytes per page */
		ret = oled_send_data(s, s->vram + p * s->width, s->width);
		if (ret) return ret;
	}
	return 0;
}

static int oled_power(struct ssd1306 *s, bool on)
{
	int ret = oled_send_cmd(s, on ? 0xAF : 0xAE);
	if (!ret) s->powered = on;
	return ret;
}

static void oled_reset(struct ssd1306 *s)
{
	if (!s->reset) return;
	gpiod_set_value_cansleep(s->reset, 1);
	usleep_range(5000, 7000);
	gpiod_set_value_cansleep(s->reset, 0);
	usleep_range(5000, 7000);
}

static int oled_init(struct ssd1306 *s)
{
	int ret = 0;
	u8 multiplex = (s->height == 64) ? 0x3F : (s->height - 1);
	u8 compins   = (s->height == 64) ? 0x12 : 0x02;

	ret |= oled_send_cmd(s, 0xAE);                 /* display off */
	ret |= oled_send_cmd2(s, 0xD5, 0x80);          /* clock div */
	ret |= oled_send_cmd2(s, 0xA8, multiplex);     /* multiplex */
	ret |= oled_send_cmd2(s, 0xD3, 0x00);          /* display offset */
	ret |= oled_send_cmd(s, 0x40);                 /* start line */
	ret |= oled_send_cmd2(s, 0x8D, 0x14);          /* charge pump on */
	ret |= oled_send_cmd2(s, 0x20, 0x00);          /* horizontal addressing */
	ret |= oled_send_cmd(s, 0xA1);                 /* seg remap */
	ret |= oled_send_cmd(s, 0xC8);                 /* com scan dec */
	ret |= oled_send_cmd2(s, 0xDA, compins);       /* compins */
	ret |= oled_send_cmd2(s, 0x81, s->contrast);   /* contrast */
	ret |= oled_send_cmd2(s, 0xD9, 0xF1);          /* precharge */
	ret |= oled_send_cmd2(s, 0xDB, 0x40);          /* vcom detect */
	ret |= oled_send_cmd(s, 0xA6);                 /* normal display */
	ret |= oled_send_cmd(s, 0xA4);                 /* display RAM */
	if (ret) return ret;

	memset(s->vram, 0x00, s->width * s->pages);
	return oled_power(s, true) ?: oled_update_all(s);
}

/* ===================== fbdev ops ===================== */

static void ssd1306_fb_update(struct ssd1306 *s, int y, int height)
{
	/* Round to page boundaries and push those pages */
	int first_page = y / 8;
	int last_page  = (y + height - 1) / 8;
	int p;
	for (p = first_page; p <= last_page; p++) {
		if (oled_set_page_col(s, p, 0))
			return;
		oled_send_data(s, s->vram + p * s->width, s->width);
	}
}

static struct ssd1306 *info_to_oled(struct fb_info *info)
{
	return info->par;
}

static ssize_t ssd1306_fb_write(struct fb_info *info, const char __user *buf,
                                size_t count, loff_t *ppos)
{
	struct ssd1306 *s = info_to_oled(info);
	size_t size = info->fix.smem_len;
	if (*ppos >= size)
		return 0;
	if (*ppos + count > size)
		count = size - *ppos;

	mutex_lock(&s->lock);
	if (copy_from_user(s->vram + *ppos, buf, count)) {
		mutex_unlock(&s->lock);
		return -EFAULT;
	}
	/* Update the whole display for simplicity */
	oled_update_all(s);
	mutex_unlock(&s->lock);

	*ppos += count;
	return count;
}

static int ssd1306_fb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	/* Provide a vmalloc-backed mapping for VRAM */
	unsigned long start = (unsigned long)info->screen_base;
	unsigned long size = info->fix.smem_len;
	return remap_vmalloc_range(vma, (void *)start, 0);
}

static void ssd1306_fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
	struct ssd1306 *s = info_to_oled(info);
	mutex_lock(&s->lock);
	cfb_fillrect(info, rect);
	ssd1306_fb_update(s, rect->dy, rect->height);
	mutex_unlock(&s->lock);
}

static void ssd1306_fb_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
	struct ssd1306 *s = info_to_oled(info);
	mutex_lock(&s->lock);
	cfb_copyarea(info, area);
	ssd1306_fb_update(s, area->dy, area->height);
	mutex_unlock(&s->lock);
}

static void ssd1306_fb_imageblit(struct fb_info *info, const struct fb_image *image)
{
	struct ssd1306 *s = info_to_oled(info);
	mutex_lock(&s->lock);
	cfb_imageblit(info, image);
	ssd1306_fb_update(s, image->dy, image->height);
	mutex_unlock(&s->lock);
}

static const struct fb_ops ssd1306_fb_ops = {
	.owner        = THIS_MODULE,
	.fb_write     = ssd1306_fb_write,
	.fb_mmap      = ssd1306_fb_mmap,
	.fb_fillrect  = ssd1306_fb_fillrect,
	.fb_copyarea  = ssd1306_fb_copyarea,
	.fb_imageblit = ssd1306_fb_imageblit,
};

/* ===================== I2C probe/remove ===================== */

static int ssd1306_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct fb_info *info;
	struct ssd1306 *s;
	int ret;

	info = framebuffer_alloc(sizeof(*s), dev);
	if (!info) return -ENOMEM;

	s = info->par;
	mutex_init(&s->lock);
	s->client = client;
	s->width  = 128;
	s->height = 64;
	s->contrast = 0x7F;
	s->col_offset = 0;

	device_property_read_u32(dev, "width",  &s->width);
	device_property_read_u32(dev, "height", &s->height);
	device_property_read_u32(dev, "col-offset", &s->col_offset);
	if (s->height % 8) {
		dev_err(dev, "height must be multiple of 8\n");
		ret = -EINVAL; goto err_fb;
	}
	s->pages = s->height / 8;

	s->reset = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(s->reset)) { ret = PTR_ERR(s->reset); goto err_fb; }

	s->vbat = devm_regulator_get_optional(dev, "vbat");
	if (IS_ERR(s->vbat)) {
		ret = PTR_ERR(s->vbat);
		if (ret == -ENODEV) s->vbat = NULL;
		else goto err_fb;
	}

	/* Allocate vmalloc VRAM: packed 1bpp but laid out as pages: width bytes * pages */
	info->fix.smem_len = s->width * s->pages;
	info->screen_base  = (void __force __iomem *)vmalloc(info->fix.smem_len);
	if (!info->screen_base) { ret = -ENOMEM; goto err_fb; }
	s->vram = (u8 *)info->screen_base;

	/* FB var/fix setup */
	strscpy(info->fix.id, "ssd1306fb", sizeof(info->fix.id));
	info->fix.type       = FB_TYPE_PACKED_PIXELS;
	info->fix.visual     = FB_VISUAL_MONO01;
	info->fix.line_length= s->width;          /* bytes per line in our page-buffer terms */
	info->fix.accel      = FB_ACCEL_NONE;

	info->var.xres = s->width;
	info->var.yres = s->height;
	info->var.xres_virtual = s->width;
	info->var.yres_virtual = s->height;
	info->var.bits_per_pixel = 1;
	info->var.nonstd = 1; /* 1bpp mono packed */

	info->fbops = &ssd1306_fb_ops;
	info->pseudo_palette = NULL;
	info->flags = FBINFO_FLAG_DEFAULT;

	i2c_set_clientdata(client, s);
	if (s->vbat) { ret = regulator_enable(s->vbat); if (ret) goto err_vfree; }

	oled_reset(s);
	ret = oled_init(s);
	if (ret) { dev_err(dev, "init failed %d\n", ret); goto err_power; }

	ret = register_framebuffer(info);
	if (ret) { dev_err(dev, "register fb failed %d\n", ret); goto err_power; }

	dev_info(dev, "SSD1306 fb registered: %dx%d, fb%d\n", s->width, s->height, info->node);
	s->info = info;
	return 0;

err_power:
	if (s->powered) oled_power(s, false);
	if (s->vbat) regulator_disable(s->vbat);
err_vfree:
	vfree((void __force void *)info->screen_base);
err_fb:
	framebuffer_release(info);
	return ret;
}

static void ssd1306_remove(struct i2c_client *client)
{
	struct ssd1306 *s = i2c_get_clientdata(client);
	if (!s || !s->info) return;
	unregister_framebuffer(s->info);
	oled_power(s, false);
	if (s->vbat) regulator_disable(s->vbat);
	vfree((void __force void *)s->info->screen_base);
	framebuffer_release(s->info);
}

#ifdef CONFIG_PM_SLEEP
static int ssd1306_suspend(struct device *dev)
{
	struct ssd1306 *s = i2c_get_clientdata(to_i2c_client(dev));
	mutex_lock(&s->lock);
	oled_power(s, false);
	mutex_unlock(&s->lock);
	return 0;
}
static int ssd1306_resume(struct device *dev)
{
	struct ssd1306 *s = i2c_get_clientdata(to_i2c_client(dev));
	int ret = 0;
	mutex_lock(&s->lock);
	oled_reset(s);
	ret = oled_init(s);
	mutex_unlock(&s->lock);
	return ret;
}
static SIMPLE_DEV_PM_OPS(ssd1306_pm, ssd1306_suspend, ssd1306_resume);
#endif

static const struct of_device_id ssd1306_of_match[] = {
	{ .compatible = "acme,ssd1306-fb" },
	{ .compatible = "acme,sh1106-fb" },
	{ }
};
MODULE_DEVICE_TABLE(of, ssd1306_of_match);

static const struct i2c_device_id ssd1306_ids[] = {
	{ "ssd1306_fb", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ssd1306_ids);

static struct i2c_driver ssd1306_driver = {
	.driver = {
		.name = DRV_NAME,
		.of_match_table = ssd1306_of_match,
#ifdef CONFIG_PM_SLEEP
		.pm = &ssd1306_pm,
#endif
	},
	.probe_new = ssd1306_probe,
	.remove    = ssd1306_remove,
	.id_table  = ssd1306_ids,
};
module_i2c_driver(ssd1306_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSD1306/SH1106 I2C fbdev driver");
MODULE_AUTHOR("MK");

