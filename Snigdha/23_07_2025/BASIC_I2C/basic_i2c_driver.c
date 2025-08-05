#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/init.h>

#define I2C_BUS_NUM 1         // I2C-1 on RPi
#define SLAVE_ADDR  0x3C      // Your I2C device address

static struct i2c_client *client;
static struct i2c_adapter *adapter;

static int __init basic_i2c_init(void)
{
    struct i2c_board_info board_info = {};
    int ret;
    u8 buffer[] = { 0x00, 0xAF };  // Example: command to turn on OLED

    pr_info("basic_i2c: Initializing \n");

    // Step 1: Get I2C adapter for bus 1
    adapter = i2c_get_adapter(I2C_BUS_NUM);
    if (!adapter) {
        pr_err("basic_i2c: Failed to get I2C adapter %d\n", I2C_BUS_NUM);
        return -ENODEV;
    }

    // Step 2: Fill board_info and create client
    strlcpy(board_info.type, "basic_i2c_dev", I2C_NAME_SIZE);
    board_info.addr = SLAVE_ADDR;

    client = i2c_new_device(adapter, &board_info);
    i2c_put_adapter(adapter);

    if (!client) {
        pr_err("basic_i2c: Failed to create I2C client\n");
        return -ENODEV;
    }

    // Step 3: Send data
    ret = i2c_master_send(client, buffer, sizeof(buffer));
    if (ret < 0) {
        pr_err("basic_i2c: Failed to send I2C data\n");
        return ret;
    }

    pr_info("basic_i2c: Data sent successfully to 0x%02x\n", SLAVE_ADDR);
    return 0;
}

static void __exit basic_i2c_exit(void)
{
    if (client)
        i2c_unregister_device(client);

    pr_info("basic_i2c: Module exited\n");
}

module_init(basic_i2c_init);
module_exit(basic_i2c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Basic I2C kernel driver without Device Tree");

