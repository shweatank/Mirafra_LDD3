#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>

#define DRV_NAME "i2c_rpi_demo"
#define SLAVE_ADDR 0x59

static int i2c_rpi_probe(struct i2c_client *client)
{
    char tx[2] = { 0x01, 0xAB };
    char reg = 0x01;
    char rx;

    pr_info("i2c_demo: Probed device at 0x%02x\n", client->addr);

    i2c_master_send(client, tx, 2);           // Write 0xAB to reg 0x01
    i2c_master_send(client, &reg, 1);         // Set register 0x01
    i2c_master_recv(client, &rx, 1);          // Read from reg 0x01

    pr_info("i2c_demo: Read 0x%02x from reg 0x01\n", rx);
    return 0;
}

static void i2c_rpi_remove(struct i2c_client *client)
{
    pr_info("i2c_demo: Device removed\n");
}

static const struct i2c_device_id i2c_rpi_id[] = {
    { DRV_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, i2c_rpi_id);

static struct i2c_driver i2c_rpi_driver = {
    .driver = {
        .name = DRV_NAME,
    },
    .probe = i2c_rpi_probe,
    .remove = i2c_rpi_remove,
    .id_table = i2c_rpi_id,
};

module_i2c_driver(i2c_rpi_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Simple I2C Driver for RPi");

