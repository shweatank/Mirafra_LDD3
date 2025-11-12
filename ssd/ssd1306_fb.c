#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DRIVER_NAME "ssd1306_fb"
#define CLASS_NAME  "ssd1306"

static struct class *ssd1306_class;
static dev_t devt;
static struct cdev ssd1306_cdev;
static struct i2c_client *ssd1306_client;

#define OLED_WIDTH 128
#define OLED_HEIGHT 32
typedef struct oled{
	int16_t width;
        int16_t height;
	u8 buffer[(128*32)/8];
}oled;
static u8 oled_buffer[(OLED_WIDTH*OLED_HEIGHT)/8];

/* ---------------- I2C Helpers ---------------- */
static int ssd1306_command(u8 cmd)
{
    u8 buf[2] = {0x00, cmd};
    return i2c_master_send(ssd1306_client, buf, 2);
}
// Update full display from buffer
static int ssd1306_update(void)
{
    int ret;
    uint8_t tx_buf[1 + OLED_WIDTH];

    if (!ssd1306_client)
        return -ENODEV;

    for (uint8_t page = 0; page < (OLED_HEIGHT / 8); page++) {
        // Set page address
        ret = ssd1306_command(0xB0 + page);
        if (ret < 0)
            return ret;

        // Set column address (lower + higher nibble)
        ret = ssd1306_command(0x00);
        if (ret < 0)
            return ret;

        ret = ssd1306_command(0x10);
        if (ret < 0)
            return ret;

        // First byte is control byte: Co=0, D/C#=1 => data stream
        tx_buf[0] = 0x40;

        // Copy one page (128 bytes) from buffer
        memcpy(&tx_buf[1], &oled_buffer[page * OLED_WIDTH], OLED_WIDTH);

        // Send data block to OLED
        ret = i2c_master_send(ssd1306_client, tx_buf, 1 + OLED_WIDTH);
        if (ret < 0)
            return ret;
    }

    return 0;
}

static void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t color) {
    int byteWidth = (w+7)/8; // Number of bytes per row
    for (int16_t j = 0; j < h; j++) {
        for (int16_t i = 0; i < w; i++) {
            if (bitmap[j * byteWidth + (i / 8)] & (128 >> (i & 7))) {
                int16_t bx = x + i;
                int16_t by = y + j;
                if (bx >= 0 && bx < OLED_WIDTH && by >= 0 && by < OLED_HEIGHT) {
                    if (color)
                        oled_buffer[bx + (by / 8) * OLED_WIDTH] |= (1 << (by & 7));
                    else
                        oled_buffer[bx + (by / 8) * OLED_WIDTH] &= ~(1 << (by & 7));
                }
            }
        }
    }
}
static int ssd1306_send_data(const oled data1, size_t len)
{
    memset(oled_buffer, 0x00, sizeof(oled_buffer));
    if(data1.width==64)
    {
	    drawBitmap(48,0,data1.buffer,data1.width,data1.height,1);
    }
    else
    {
             drawBitmap(0, 0, data1.buffer, data1.width, data1.height, 1);
    }
    ssd1306_update();
    return 0;
}
/* ---------------- Char Device Ops ---------------- */
static ssize_t ssd1306_write(struct file *file, const char __user *buf,
                             size_t count, loff_t *ppos)
{
    oled data1;
    if (count > sizeof(oled))
        count = sizeof(oled);
	
    if (copy_from_user(&data1, buf, count))
        return -EFAULT;

    ssd1306_send_data(data1, sizeof(oled));

    return count;
}

static const struct file_operations ssd1306_fops = {
    .owner = THIS_MODULE,
    .write = ssd1306_write,
};

/* ---------------- Probe & Remove ---------------- */
static int ssd1306_probe(struct i2c_client *client)
{
    int ret;

    ssd1306_client = client;

    /* Init OLED */
    ssd1306_command(0xAE); // Display off
    ssd1306_command(0xA8); ssd1306_command(0x1F); // Multiplex ratio (32px)
    ssd1306_command(0xD3); ssd1306_command(0x00); // Display offset
    ssd1306_command(0x40); // Start line
    ssd1306_command(0xA1); // Segment remap
    ssd1306_command(0xC8); // COM scan direction
    ssd1306_command(0xDA); ssd1306_command(0x02); // COM pins
    ssd1306_command(0x81); ssd1306_command(0x8F); // Contrast
    ssd1306_command(0xA4); // Resume to RAM content
    ssd1306_command(0xA6); // Normal display
    ssd1306_command(0xD5); ssd1306_command(0x80); // Clock
    ssd1306_command(0x8D); ssd1306_command(0x14); // Enable charge pump
    ssd1306_command(0xAF); // Display ON

    /* Char device register */
    ret = alloc_chrdev_region(&devt, 0, 1, DRIVER_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&ssd1306_cdev, &ssd1306_fops);
    ret = cdev_add(&ssd1306_cdev, devt, 1);
    if (ret < 0)
        goto err_unregister;

    ssd1306_class = class_create(CLASS_NAME);
    if (IS_ERR(ssd1306_class)) {
        ret = PTR_ERR(ssd1306_class);
        goto err_cdev_del;
    }

    device_create(ssd1306_class, NULL, devt, NULL, DRIVER_NAME);

    dev_info(&client->dev, "SSD1306 probed successfully\n");
    return 0;

err_cdev_del:
    cdev_del(&ssd1306_cdev);
err_unregister:
    unregister_chrdev_region(devt, 1);
    return ret;
}

static void ssd1306_remove(struct i2c_client *client)
{
    device_destroy(ssd1306_class, devt);
    class_destroy(ssd1306_class);
    cdev_del(&ssd1306_cdev);
    unregister_chrdev_region(devt, 1);

    dev_info(&client->dev, "SSD1306 removed\n");
}

/* ---------------- ID Table & Driver ---------------- */
static const struct of_device_id ssd1306_of_match[] = {
    { .compatible = "pavan,ssd1306-fb" },
    { }
};
MODULE_DEVICE_TABLE(of, ssd1306_of_match);

static struct i2c_driver ssd1306_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = ssd1306_of_match,
    },
    .probe = ssd1306_probe,  
    .remove = ssd1306_remove,
};

module_i2c_driver(ssd1306_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pavan");
MODULE_DESCRIPTION("SSD1306 framebuffer char driver");

