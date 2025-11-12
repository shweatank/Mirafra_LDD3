// ili9225_fb.c - SPI framebuffer + char device driver for ILI9225 LCD
//
// This driver does two things:
//  1. Registers a framebuffer device (/dev/fb0) for Linux apps.
//  2. Registers a character device   (/dev/ili9225) for direct frame pushing
//     from user programs (like ili9225_player.c).
//
// Whenever you write() a frame to /dev/ili9225, the data is copied to fb0
// and also pushed out to the LCD via SPI.

#include <linux/module.h>     // Required for all kernel modules
#include <linux/kernel.h>     // Kernel helper macros (pr_info, printk, etc.)
#include <linux/spi/spi.h>    // SPI subsystem APIs
#include <linux/gpio/consumer.h> // GPIO descriptor API (gpiod_)
#include <linux/fb.h>         // Linux framebuffer APIs
#include <linux/uaccess.h>    // copy_from_user, copy_to_user, memdup_user
#include <linux/delay.h>      // msleep, udelay
#include <linux/cdev.h>       // Char device APIs (cdev, alloc_chrdev_region)
#include <linux/device.h>     // Device model (class_create, device_create)


#define DRIVER_NAME "ili9225_fb"   // Kernel driver name
#define DEVICE_NAME "ili9225"      // Char device name (/dev/ili9225)
#define LCD_WIDTH   176            // LCD resolution width
#define LCD_HEIGHT  220            // LCD resolution height
#define FRAME_SIZE  (LCD_WIDTH * LCD_HEIGHT * 2) // 16bpp RGB565 frame size

// ---------------- Device context ----------------
struct ili9225_fb {
    struct spi_device *spi;        // SPI device handle
    struct gpio_desc *reset;       // RESET pin
    struct gpio_desc *dc;          // Data/Command pin
    struct fb_info *info;          // Linux framebuffer info

    dev_t devt;                    // Char device number
    struct cdev cdev;              // Char device structure
    struct class *class;           // Sysfs class for /dev/ili9225
};

// ---------------- SPI helpers ----------------

// Send a command byte
static inline int ili9225_write_cmd(struct ili9225_fb *lcd, u8 cmd)
{
    gpiod_set_value_cansleep(lcd->dc, 0); // DC=0 → command
    return spi_write(lcd->spi, &cmd, 1);  // Send 1 byte over SPI
}

// Send 16-bit data (RGB565 pixel or register value)
static inline int ili9225_write_data16(struct ili9225_fb *lcd, u16 data)
{
    u8 buf[2] = { data >> 8, data & 0xFF }; // High byte + Low byte
    gpiod_set_value_cansleep(lcd->dc, 1);   // DC=1 → data
    return spi_write(lcd->spi, buf, 2);
}

// ---------------- LCD init ----------------
static int ili9225_init(struct ili9225_fb *lcd)
{
    // Hardware reset sequence (toggle reset pin)
    if (lcd->reset) {
        gpiod_set_value_cansleep(lcd->reset, 1);
        msleep(10);
        gpiod_set_value_cansleep(lcd->reset, 0);
        msleep(50);
        gpiod_set_value_cansleep(lcd->reset, 1);
        msleep(120);
    }

    
    ili9225_write_cmd(lcd, 0x01); // Driver output ctrl
    ili9225_write_data16(lcd, 0x011C);
    ili9225_write_cmd(lcd, 0x07); // Display ON
    ili9225_write_data16(lcd, 0x1017);

    return 0;
}

// ---------------- Framebuffer ops ----------------


// Check video mode validity
static int ili9225_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
    if (var->xres != LCD_WIDTH || var->yres != LCD_HEIGHT)
        return -EINVAL;
    if (var->bits_per_pixel != 16)
        return -EINVAL;
    return 0;
}


// Framebuffer operation table
static struct fb_ops ili9225_fb_ops = {
    .owner        = THIS_MODULE,
    .fb_check_var = ili9225_fb_check_var,
};

// ---------------- Char device ops ----------------

// Write handler for /dev/ili9225
static ssize_t ili9225_write(struct file *file,
                             const char __user *buf,
                             size_t count, loff_t *ppos)
{
    struct ili9225_fb *lcd = file->private_data;
    u8 *kbuf;
    int ret;

    // Must be full frame
    if (count != FRAME_SIZE)
        return -EINVAL;

    // Copy frame from userspace
    kbuf = memdup_user(buf, count);
    if (IS_ERR(kbuf))
        return PTR_ERR(kbuf);

    // Copy into fb0 system buffer (so Linux apps see latest frame)
    memcpy(lcd->info->screen_base, kbuf, count);

    // Push frame to LCD hardware
    ili9225_write_cmd(lcd, 0x20); // Set X=0
    ili9225_write_data16(lcd, 0);
    ili9225_write_cmd(lcd, 0x21); // Set Y=0
    ili9225_write_data16(lcd, 0);
    ili9225_write_cmd(lcd, 0x22); // Start GRAM write

    gpiod_set_value_cansleep(lcd->dc, 1);
    ret = spi_write(lcd->spi, kbuf, count);

    kfree(kbuf);
    if (ret < 0)
        return ret;

    return count;
}

// Open handler for /dev/ili9225
static int ili9225_open(struct inode *inode, struct file *file)
{
    struct ili9225_fb *lcd =
        container_of(inode->i_cdev, struct ili9225_fb, cdev);
    file->private_data = lcd;
    return 0;
}

// Char device fops table
static const struct file_operations ili9225_fops = {
    .owner = THIS_MODULE,
    .open  = ili9225_open,
    .write = ili9225_write,
};

// ---------------- Probe / Remove ----------------
static int ili9225_probe(struct spi_device *spi)
{
    struct ili9225_fb *lcd;
    struct fb_info *info;
    int ret;

    // Allocate driver context
    lcd = devm_kzalloc(&spi->dev, sizeof(*lcd), GFP_KERNEL);
    if (!lcd)
        return -ENOMEM;
    lcd->spi = spi;

    // Request GPIOs
    lcd->reset = devm_gpiod_get(&spi->dev, "reset", GPIOD_OUT_HIGH);
    lcd->dc    = devm_gpiod_get(&spi->dev, "dc", GPIOD_OUT_LOW);
    if (IS_ERR(lcd->reset) || IS_ERR(lcd->dc))
        return -ENODEV;

    // Initialize LCD
    ili9225_init(lcd);

    // Allocate framebuffer info structure
    info = framebuffer_alloc(0, &spi->dev);
    if (!info)
        return -ENOMEM;

    // Fill fb_info fields
    info->fbops = &ili9225_fb_ops;
    info->screen_base = devm_kzalloc(&spi->dev, FRAME_SIZE, GFP_KERNEL);
    info->fix.smem_len = FRAME_SIZE;
    info->fix.line_length = LCD_WIDTH * 2;
    snprintf(info->fix.id, sizeof(info->fix.id), "ILI9225");
    info->var.xres = LCD_WIDTH;
    info->var.yres = LCD_HEIGHT;
    info->var.bits_per_pixel = 16;
    info->var.red.offset = 11; info->var.red.length = 5;
    info->var.green.offset = 5; info->var.green.length = 6;
    info->var.blue.offset = 0; info->var.blue.length = 5;

    lcd->info = info;

    // Register framebuffer → /dev/fb0
    ret = register_framebuffer(info);
    if (ret < 0)
        return ret;

    // Register char device → /dev/ili9225
    ret = alloc_chrdev_region(&lcd->devt, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;
    cdev_init(&lcd->cdev, &ili9225_fops);
    lcd->cdev.owner = THIS_MODULE;
    cdev_add(&lcd->cdev, lcd->devt, 1);
    lcd->class = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(lcd->class, NULL, lcd->devt, NULL, DEVICE_NAME);

    spi_set_drvdata(spi, lcd);

    pr_info(DRIVER_NAME ": fb0 + char dev registered\n");
    return 0;
}

static void ili9225_remove(struct spi_device *spi)
{
    struct ili9225_fb *lcd = spi_get_drvdata(spi);

    // Remove char device
    device_destroy(lcd->class, lcd->devt);
    class_destroy(lcd->class);
    cdev_del(&lcd->cdev);
    unregister_chrdev_region(lcd->devt, 1);

    // Remove framebuffer
    unregister_framebuffer(lcd->info);
    framebuffer_release(lcd->info);

    pr_info(DRIVER_NAME ": removed\n");
}

// ---------------- Device Tree match ----------------
static const struct of_device_id ili9225_of_match[] = {
    { .compatible = "ilitek,ili9225" },
    { }
};
MODULE_DEVICE_TABLE(of, ili9225_of_match);

// SPI driver struct
static struct spi_driver ili9225_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = ili9225_of_match,
    },
    .probe  = ili9225_probe,
    .remove = ili9225_remove,
};

module_spi_driver(ili9225_driver);

MODULE_DESCRIPTION("Framebuffer + char device driver for ILI9225 LCD");
MODULE_LICENSE("GPL");

