#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio/consumer.h>
#include <linux/device.h>
#include <linux/delay.h>
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define MAX_TEXT    128

#define DEVICE_NAME "oled_spi"

static struct spi_device *oled_spi;
static struct gpio_desc *dc_gpio, *rst_gpio;
static int major;
static struct class *oled_class;
static char oled_text[MAX_TEXT];

extern const u8 font8x8_basic[128][8];

// Send Command
static int oled_cmd(u8 cmd) {
    gpiod_set_value(dc_gpio, 0);  // Command mode
    return spi_write(oled_spi, &cmd, 1);
}

// Send Data
static int oled_data(u8 *data, size_t len) {
    gpiod_set_value(dc_gpio, 1);  // Data mode
    return spi_write(oled_spi, data, len);
}

// Reset OLED
static void oled_reset(void) {
    gpiod_set_value(rst_gpio, 0);
    msleep(50);
    gpiod_set_value(rst_gpio, 1);
    msleep(50);
}

// OLED Init Sequence (SSD1306)
static void oled_init(void) {
    oled_reset();
    oled_cmd(0xAE);
    oled_cmd(0xD5); oled_cmd(0x80);
    oled_cmd(0xA8); oled_cmd(0x3F);
    oled_cmd(0xD3); oled_cmd(0x00);
    oled_cmd(0x40);
    oled_cmd(0x8D); oled_cmd(0x14);
    oled_cmd(0x20); oled_cmd(0x00);
    oled_cmd(0xA1);
    oled_cmd(0xC8);
    oled_cmd(0xDA); oled_cmd(0x12);
    oled_cmd(0x81); oled_cmd(0xCF);
    oled_cmd(0xD9); oled_cmd(0xF1);
    oled_cmd(0xDB); oled_cmd(0x40);
    oled_cmd(0xA4);
    oled_cmd(0xA6);
    oled_cmd(0xAF);
}

// Clear screen
static void oled_clear(void) {
    u8 zero[OLED_WIDTH] = {0};
    for (int page = 0; page < 8; page++) {
        oled_cmd(0xB0 + page); // Page address
        oled_cmd(0x00);        // Column low
        oled_cmd(0x10);        // Column high
        oled_data(zero, OLED_WIDTH);
    }
}

// Display character
static void oled_putc(char c) {
    if (c < 0 || c > 127) c = ' ';
    oled_data((u8 *)font8x8_basic[(int)c], 8);
}

// Display string
static void oled_display(const char *str) {
    oled_clear();
    oled_cmd(0xB0); // Page 0
    oled_cmd(0x00);
    oled_cmd(0x10);
    while (*str) oled_putc(*str++);
}

// File write from user
static ssize_t oled_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
    if (len > MAX_TEXT - 1) len = MAX_TEXT - 1;
    if (copy_from_user(oled_text, buf, len)) return -EFAULT;
    oled_text[len] = '\0';
    oled_display(oled_text);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = oled_write,
};

// Probe
static int oled_probe(struct spi_device *spi) {
    int ret;

    oled_spi = spi;

    dc_gpio = gpiod_get(&spi->dev, "dc", GPIOD_OUT_LOW);
    if (IS_ERR(dc_gpio)) return PTR_ERR(dc_gpio);

    rst_gpio = gpiod_get(&spi->dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(rst_gpio)) return PTR_ERR(rst_gpio);

    major = register_chrdev(0, DEVICE_NAME, &fops);
    oled_class = class_create("oled_class");
    device_create(oled_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    oled_init();
    oled_display("OLED Ready");

    printk(KERN_INFO "OLED SPI Driver Loaded.\n");
    return 0;
}

// Remove
static void oled_remove(struct spi_device *spi) {
    oled_clear();
    device_destroy(oled_class, MKDEV(major, 0));
    class_destroy(oled_class);
    unregister_chrdev(major, DEVICE_NAME);
    gpiod_put(dc_gpio);
    gpiod_put(rst_gpio);
    printk(KERN_INFO "OLED SPI Driver Unloaded.\n");
    
}

static const struct spi_device_id oled_id[] = {
    { "oled_spi", 0 },
    { }
};
MODULE_DEVICE_TABLE(spi, oled_id);

static struct spi_driver oled_driver = {
    .driver = {
        .name = "oled_spi",
        .owner = THIS_MODULE,
    },
    .probe = oled_probe,
    .remove = oled_remove,
    .id_table = oled_id,
};

module_spi_driver(oled_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("SPI OLED Driver with Font Support");
