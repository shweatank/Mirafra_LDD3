#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#define SSD1306_ADDR 0x3C
#define WIDTH 128
#define HEIGHT 32
#define PAGES (HEIGHT / 8)

static struct i2c_client *oled_client;

static const uint8_t font5x8[][5] = {
    {0x7E,0x11,0x11,0x7E,0x00}, // A
    {0x7F,0x49,0x49,0x36,0x00}, // B
    {0x3E,0x41,0x41,0x22,0x00}, // C
    {0x7F,0x41,0x41,0x3E,0x00}, // D
    {0x7F,0x49,0x49,0x41,0x00}, // E
    {0x7F,0x09,0x09,0x01,0x00}, // F
    {0x3E,0x41,0x51,0x72,0x00}, // G
    {0x7F,0x08,0x08,0x7F,0x00}, // H
    {0x41,0x7F,0x41,0x00,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x00}, // J
    {0x7F,0x08,0x14,0x63,0x00}, // K
    {0x7F,0x40,0x40,0x40,0x00}, // L
    {0x7F,0x02,0x04,0x02,0x7F}, // M
    {0x7F,0x06,0x18,0x7F,0x00}, // N
    {0x3E,0x41,0x41,0x3E,0x00}, // O
    {0x7F,0x09,0x09,0x06,0x00}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x66,0x00}, // R
    {0x26,0x49,0x49,0x32,0x00}, // S
    {0x01,0x7F,0x01,0x00,0x00}, // T
    {0x3F,0x40,0x40,0x3F,0x00}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x30,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
    {0x00,0x00,0x00,0x00,0x00}  // space
};

static void ssd1306_write_cmd(uint8_t cmd)
{
    uint8_t buf[2] = {0x00, cmd};
    i2c_master_send(oled_client, buf, 2);
}

static void ssd1306_write_data(uint8_t data)
{
    uint8_t buf[2] = {0x40, data};
    i2c_master_send(oled_client, buf, 2);
}

static void ssd1306_set_cursor(uint8_t col, uint8_t page)
{
    ssd1306_write_cmd(0xB0 + page);
    ssd1306_write_cmd(0x00 + (col & 0x0F));
    ssd1306_write_cmd(0x10 + (col >> 4));
}

static void ssd1306_clear(void)
{
    for (uint8_t page = 0; page < PAGES; page++) {
        ssd1306_set_cursor(0, page);
        for (int col = 0; col < WIDTH; col++)
            ssd1306_write_data(0x00);
    }
}

static void ssd1306_init(void)
{
    static const uint8_t init_seq[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x1F,
        0xD3, 0x00, 0x40, 0x8D, 0x14,
        0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x02,
        0x81, 0x8F, 0xD9, 0xF1, 0xDB, 0x40,
        0xA4, 0xA6, 0xAF
    };
    for (int i = 0; i < ARRAY_SIZE(init_seq); i++)
        ssd1306_write_cmd(init_seq[i]);
    msleep(100);
}

static void ssd1306_draw_char(char c)
{
    const uint8_t *bitmap;
    if (c >= 'A' && c <= 'Z')
        bitmap = font5x8[c - 'A'];
    else
        bitmap = font5x8[26];  // space

    for (int i = 0; i < 5; i++)
        ssd1306_write_data(bitmap[i]);
    ssd1306_write_data(0x00);  // spacing
}

static void ssd1306_draw_string(const char *str)
{
    ssd1306_set_cursor(0, 0);
    while (*str)
        ssd1306_draw_char(*str++);
}

static int ssd1306_probe(struct i2c_client *client)
{
    oled_client = client;
    dev_info(&client->dev, "SSD1306 OLED 128x32 Detected\n");

    ssd1306_init();
    ssd1306_clear();
    ssd1306_draw_string("PLAYED SUCCESSFULLY");

    return 0;
}

static void ssd1306_remove(struct i2c_client *client)
{
    dev_info(&client->dev, "SSD1306 OLED Removed\n");
    ssd1306_clear();
}

static const struct of_device_id ssd1306_dt_ids[] = {
    { .compatible = "my_i2c,ssd1306" },
    { }
};
MODULE_DEVICE_TABLE(of, ssd1306_dt_ids);

static struct i2c_driver ssd1306_driver = {
    .driver = {
        .name = "ssd1306_kernel_text",
        .of_match_table = ssd1306_dt_ids,
    },
    .probe = ssd1306_probe,
    .remove = ssd1306_remove,
};

module_i2c_driver(ssd1306_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priya");
MODULE_DESCRIPTION("SSD1306 Kernel Driver (128x32) - Display Text");

