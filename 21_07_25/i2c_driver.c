#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>

#define DRV_NAME "direct_i2c"
#define SLAVE_ADDR 0x56 // Example: EEPROM at 0x50

static struct i2c_client *demo_client;
static struct i2c_board_info board_info = {
    I2C_BOARD_INFO(DRV_NAME, SLAVE_ADDR)
};

static int __init direct_i2c_init(void)
{
    struct i2c_adapter *adap;
    char data;
    adap = i2c_get_adapter(1); // I2C bus 1 (Raspberry Pi)
    if (!adap)
        return -ENODEV;

    demo_client = i2c_new_client_device(adap, &board_info);
    i2c_put_adapter(adap);

    if (IS_ERR(demo_client))
        return PTR_ERR(demo_client);

    pr_info("direct_i2c: Client created at 0x%x\n", demo_client->addr);
    
    // Write example: send 0xAA to reg 0x00
    char tx[2] = {0x00, 0xAA};
    i2c_master_send(demo_client, tx, 2);
  
    return 0;
}

static void __exit direct_i2c_exit(void)
{
    if (demo_client) {
        i2c_unregister_device(demo_client);
        pr_info("direct_i2c: Client unregistered\n");
    }
}

module_init(direct_i2c_init);
module_exit(direct_i2c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sowmya");
MODULE_DESCRIPTION("Direct I2C client creation example");

