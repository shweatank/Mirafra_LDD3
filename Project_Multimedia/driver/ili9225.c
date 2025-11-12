#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>

#define DRIVER_NAME "ili9225"    // Kernel driver name
#define DEVICE_NAME "ili9225"    // Name of /dev node

/* Global variables for char device handling */
static dev_t devt;              // Device number (major+minor)
static struct cdev ili_cdev;    // Character device structure
static struct class *ili_class; // Sysfs class for auto /dev creation
static struct spi_device *ili9225_spi; // Global SPI device pointer

/* Private structure for ili9225 driver instance */
struct ili9225 {
    struct spi_device *spi; // SPI device handle
    struct gpio_desc *reset; // GPIO for RESET line
    struct gpio_desc *dc;    // GPIO for Data/Command line
    u16 pw; // panel width (pixels)
    u16 ph; // panel height (pixels)
};


/* Send a command byte (DC = 0) */
static inline int ili9225_write_cmd(struct ili9225 *lcd, u8 cmd)
{
    gpiod_set_value_cansleep(lcd->dc, 0); // Set DC = 0 → command mode
    return spi_write(lcd->spi, &cmd, 1);  // Send 1-byte command over SPI
}

/* Send one data byte (DC = 1) */
static inline int ili9225_write_data8(struct ili9225 *lcd, u8 data)
{
    gpiod_set_value_cansleep(lcd->dc, 1); // Set DC = 1 → data mode
    return spi_write(lcd->spi, &data, 1); // Send 1-byte data over SPI
}

/* Send one data word (16-bit, high byte first) */
static inline int ili9225_write_data16(struct ili9225 *lcd, u16 data)
{
    u8 buf[2] = { data >> 8, data & 0xFF }; // Split into two bytes
    gpiod_set_value_cansleep(lcd->dc, 1);   // Data mode
    return spi_write(lcd->spi, buf, 2);     // Send 2 bytes over SPI
}

/* Write register = command + 16-bit value */
static int ili9225_write_reg(struct ili9225 *lcd, u8 reg, u16 val)
{
    int ret = ili9225_write_cmd(lcd, reg); // Send register index
    if (ret)
        return ret;
    return ili9225_write_data16(lcd, val); // Send register value
}

//LCD Initialization Sequence

/*
 * Initialization sequence for ILI9225 controller in RGB565 mode.
 * This configures power, timing, frame rate, GRAM area, and gamma.
 */
static int ili9225_init(struct ili9225 *lcd)
{
    /* Hardware reset via GPIO (if available) */
    if (lcd->reset) {
        gpiod_set_value_cansleep(lcd->reset, 1);
        msleep(10);
        gpiod_set_value_cansleep(lcd->reset, 0);
        msleep(50);
        gpiod_set_value_cansleep(lcd->reset, 1);
        msleep(120);
    }

    /* Power-down sequence */
    ili9225_write_reg(lcd, 0x10, 0x0000);
    ili9225_write_reg(lcd, 0x11, 0x0000);
    ili9225_write_reg(lcd, 0x12, 0x0000);
    ili9225_write_reg(lcd, 0x13, 0x0000);
    ili9225_write_reg(lcd, 0x14, 0x0000);
    usleep_range(40000, 41000);

    /* Power-on sequence */
    ili9225_write_reg(lcd, 0x11, 0x0018);
    ili9225_write_reg(lcd, 0x12, 0x6121);
    ili9225_write_reg(lcd, 0x13, 0x006F);
    ili9225_write_reg(lcd, 0x14, 0x495F);
    ili9225_write_reg(lcd, 0x10, 0x0800);
    usleep_range(10000, 11000);

    ili9225_write_reg(lcd, 0x11, 0x103B);
    usleep_range(50000, 51000);

    /* Driver output settings */
    ili9225_write_reg(lcd, 0x01, 0x011C);
    ili9225_write_reg(lcd, 0x02, 0x0100);
    ili9225_write_reg(lcd, 0x03, 0x1030);
    ili9225_write_reg(lcd, 0x07, 0x0000);

    /* Frame rate */
    ili9225_write_reg(lcd, 0x08, 0x0808);

    /* Entry mode */
    ili9225_write_reg(lcd, 0x0F, 0x0801);
    ili9225_write_reg(lcd, 0x0B, 0x1100);

    /* Define GRAM (frame buffer) area */
    ili9225_write_reg(lcd, 0x36, 0x00AF); // X end = 175
    ili9225_write_reg(lcd, 0x37, 0x0000); // X start = 0
    ili9225_write_reg(lcd, 0x38, 0x00DB); // Y end = 219
    ili9225_write_reg(lcd, 0x39, 0x0000); // Y start = 0
    ili9225_write_reg(lcd, 0x20, 0x0000); // RAM X address
    ili9225_write_reg(lcd, 0x21, 0x0000); // RAM Y address

    /* Gamma correction (values depend on panel) */
    ili9225_write_reg(lcd, 0x30, 0x0000);
    ili9225_write_reg(lcd, 0x31, 0x00DB);
    ili9225_write_reg(lcd, 0x32, 0x0000);
    ili9225_write_reg(lcd, 0x33, 0x0000);
    ili9225_write_reg(lcd, 0x34, 0x00DB);
    ili9225_write_reg(lcd, 0x35, 0x0000);
    ili9225_write_reg(lcd, 0x36, 0x00AF);
    ili9225_write_reg(lcd, 0x37, 0x0000);
    ili9225_write_reg(lcd, 0x38, 0x00DB);
    ili9225_write_reg(lcd, 0x39, 0x0000);

    /* Turn on display */
    ili9225_write_reg(lcd, 0x07, 0x1017);

    return 0;
}


static void ili9225_set_window(struct ili9225 *lcd, u16 xs, u16 ys, u16 xe, u16 ye)
{
    ili9225_write_reg(lcd, 0x36, xe);
    ili9225_write_reg(lcd, 0x37, xs);
    ili9225_write_reg(lcd, 0x38, ye);
    ili9225_write_reg(lcd, 0x39, ys);

    /* Set RAM address pointer */
    ili9225_write_reg(lcd, 0x20, xs);
    ili9225_write_reg(lcd, 0x21, ys);

    /* Next data goes into GRAM */
    ili9225_write_cmd(lcd, 0x22);
}

/* Display a full RGB565 frame */
static void ili9225_display_rgb565(struct ili9225 *lcd, u8 *frame565)
{
    u16 x, y;
    u8 row_buf[lcd->pw * 2]; // one row (176 pixels * 2 bytes = 352B)

    /* Set window to entire screen */
    ili9225_set_window(lcd, 0, 0, lcd->pw - 1, lcd->ph - 1);

    gpiod_set_value_cansleep(lcd->dc, 1); // Data mode

    /* Send one row at a time */
    for (y = 0; y < lcd->ph; y++) {
        for (x = 0; x < lcd->pw; x++) {
            /* LCD expects big-endian 565: swap bytes */
            row_buf[2*x]     = frame565[2*(y*lcd->pw + x) + 1]; // LSB
            row_buf[2*x + 1] = frame565[2*(y*lcd->pw + x)];     // MSB
        }
        spi_write(lcd->spi, row_buf, lcd->pw * 2);
    }
}

/* ------------------- Character Device Ops ------------------- */

/* Open: initialize LCD */
static int ili9225_open(struct inode *inode, struct file *file)
{
    struct ili9225 *lcd = spi_get_drvdata(ili9225_spi);
    if (!lcd)
        return -ENODEV;

    int ret = ili9225_init(lcd);
    if (ret)
        dev_err(&lcd->spi->dev, "LCD init failed\n");

    return 0;
}

/* Release: just log */
static int ili9225_release(struct inode *inode, struct file *file)
{
    pr_info("ili9225: device closed\n");
    return 0;
}

/* Write: user-space writes a raw RGB565 frame buffer */
static ssize_t ili9225_write(struct file *file, const char __user *buf,
                             size_t len, loff_t *off)
{
    struct ili9225 *lcd = spi_get_drvdata(ili9225_spi);
    u8 *kbuf;
    size_t expected_size;

    if (!lcd)
        return -ENODEV;

    expected_size = lcd->pw * lcd->ph * 2; // width * height * 2 bytes/pixel

    /* Validate input size */
    if (len != expected_size) {
        pr_err("ili9225: expected %zu bytes, got %zu\n",
               expected_size, len);
        return -EINVAL;
    }

    /* Allocate kernel buffer for frame */
    kbuf = kmalloc(len, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    /* Copy from user-space to kernel buffer */
    if (copy_from_user(kbuf, buf, len)) {
        kfree(kbuf);
        return -EFAULT;
    }

    /* Send frame to LCD */
    ili9225_display_rgb565(lcd, kbuf);

    kfree(kbuf);
    return len;
}

/* File operations table */
static const struct file_operations ili9225_fops = {
    .owner   = THIS_MODULE,
    .open    = ili9225_open,
    .release = ili9225_release,
    .write   = ili9225_write,
};


static int ili9225_probe(struct spi_device *spi)
{
    struct ili9225 *lcd;
    int ret;

    pr_info(DRIVER_NAME ": probe called for %s\n", dev_name(&spi->dev));

    /* Allocate driver private data */
    lcd = devm_kzalloc(&spi->dev, sizeof(*lcd), GFP_KERNEL);
    if (!lcd)
        return -ENOMEM;

    lcd->spi = spi;
    lcd->pw = 176; // Panel resolution X
    lcd->ph = 220; // Panel resolution Y

    /* Request GPIOs from device tree */
    lcd->reset = devm_gpiod_get(&spi->dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(lcd->reset)) {
        dev_err(&spi->dev, "Failed to get reset GPIO\n");
        return PTR_ERR(lcd->reset);
    }

    lcd->dc = devm_gpiod_get(&spi->dev, "dc", GPIOD_OUT_LOW);
    if (IS_ERR(lcd->dc)) {
        dev_err(&spi->dev, "Failed to get dc GPIO\n");
        return PTR_ERR(lcd->dc);
    }

    /* Perform hardware reset */
    gpiod_set_value(lcd->reset, 0);
    msleep(20);
    gpiod_set_value(lcd->reset, 1);
    msleep(120);

    /* Save driver data for later */
    spi_set_drvdata(spi, lcd);
    ili9225_spi = spi;

    /* Create /dev/ili9225 device */
    ret = alloc_chrdev_region(&devt, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&ili_cdev, &ili9225_fops);
    ili_cdev.owner = THIS_MODULE;

    ret = cdev_add(&ili_cdev, devt, 1);
    if (ret) {
        unregister_chrdev_region(devt, 1);
        return ret;
    }

    ili_class = class_create(DEVICE_NAME);
    if (IS_ERR(ili_class)) {
        cdev_del(&ili_cdev);
        unregister_chrdev_region(devt, 1);
        return PTR_ERR(ili_class);
    }

    device_create(ili_class, NULL, devt, NULL, DEVICE_NAME);

    pr_info(DRIVER_NAME ": /dev/%s created\n", DEVICE_NAME);

    return 0;
}

static void ili9225_remove(struct spi_device *spi)
{
    pr_info(DRIVER_NAME ": remove called\n");

    /* Remove /dev device */
    device_destroy(ili_class, devt);
    class_destroy(ili_class);

    /* Delete char device and free dev number */
    cdev_del(&ili_cdev);
    unregister_chrdev_region(devt, 1);

    /* Clear SPI drvdata */
    spi_set_drvdata(spi, NULL);

    pr_info(DRIVER_NAME ": device removed\n");
}

static const struct of_device_id ili9225_of_match[] = {
    { .compatible = "ilitek,ili9225" },
    { }
};
MODULE_DEVICE_TABLE(of, ili9225_of_match);

static struct spi_driver ili9225_driver = {
    .driver = {
        .name   = "ili9225",
        .of_match_table = ili9225_of_match,
    },
    .probe  = ili9225_probe,
    .remove = ili9225_remove,
};
module_spi_driver(ili9225_driver);

MODULE_AUTHOR("Group - 1");
MODULE_DESCRIPTION("ILI9225 display driver");
MODULE_LICENSE("GPL");

