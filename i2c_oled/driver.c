#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/kernel.h>

#define OLED_ADDR 0x3C

static struct i2c_adapter *adapter = NULL;
static struct i2c_client *client = NULL;

static u8 oled_init_seq[] = {
    0xAE,       // Display OFF
    0xD5, 0x80, // Set display clock divide ratio
    0xA8, 0x3F, // Set multiplex ratio (1 to 64)
    0xD3, 0x00, // Display offset
    0x40,       // Start line address
    0x8D, 0x14, // Charge Pump
    0x20, 0x00, // Memory addressing mode: Horizontal
    0xA1,       // Segment remap
    0xC8,       // COM output scan direction
    0xDA, 0x12, // COM pins
    0x81, 0xCF, // Contrast
    0xD9, 0xF1, // Pre-charge
    0xDB, 0x40, // VCOMH
    0xA4,       // Resume RAM display
    0xA6,       // Normal display
    0xAF        // Display ON
};

static int send_oled_command(struct i2c_client *client, u8 cmd)
{
    u8 buffer[2] = { 0x00, cmd };  // 0x00 = command mode
    int ret = i2c_master_send(client, buffer, 2);
    if (ret < 0)
        pr_err("OLED: Failed to send cmd 0x%02x\n", cmd);
    return ret;
}

static int basic_i2c_init(void)
{
    struct i2c_board_info board_info = {};
    int ret, i;

    pr_info("basic_i2c: Initializing OLED driver...\n");

    adapter = i2c_get_adapter(1); // Use I2C bus 1
    if (!adapter) {
        pr_err("basic_i2c: Failed to get adapter\n");
        return -ENODEV;
    }

    strncpy(board_info.type, "ssd1306", I2C_NAME_SIZE);
    board_info.addr = OLED_ADDR;

    client = i2c_new_client_device(adapter, &board_info);
    i2c_put_adapter(adapter);

    if (IS_ERR(client)) {
        pr_err("basic_i2c: Failed to create I2C client\n");
        return PTR_ERR(client);
    }

    pr_info("basic_i2c: Sending OLED init sequence...\n");

    for (i = 0; i < sizeof(oled_init_seq); i++) {
        ret = send_oled_command(client, oled_init_seq[i]);
        if (ret < 0) {
            pr_err("basic_i2c: OLED init failed at cmd index %d\n", i);
            i2c_unregister_device(client);
            return ret;
        }
    }

    pr_info("basic_i2c: OLED init sequence completed\n");
    return 0;
}

static void basic_i2c_exit(void)
{
    pr_info("basic_i2c: Exiting module and unregistering device\n");
    if (client)
        i2c_unregister_device(client);
}

module_init(basic_i2c_init);
module_exit(basic_i2c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Basic I2C OLED Driver for SSD1306 without Device Tree");
