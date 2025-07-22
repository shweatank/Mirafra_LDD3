#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include<linux/delay.h>

#define DRV_NAME    "eeprom_demo"
#define EEPROM_ADDR 0x54 // EEPROM I2C address (check i2cdetect)

static int eeprom_probe(struct i2c_client *client)
{
    char mem_addr = 0x00;    // EEPROM memory address
    char data_w = 0x5A;      // Example data to write
    char data_r;

    // Write 0x5A to EEPROM at memory location 0x00
    char tx[2] = { mem_addr, data_w };
    i2c_master_send(client, tx, 2);

    // Small delay (some EEPROMs require write cycle time)
    fsleep(10);

    // Read back from same location
    i2c_master_send(client, &mem_addr, 1);
    i2c_master_recv(client, &data_r, 1);

    pr_info("EEPROM: Written 0x%02x, Read back 0x%02x\n", data_w, data_r);
    return 0;
}

static void eeprom_remove(struct i2c_client *client)
{
    pr_info("EEPROM: Removed device\n");
}

static const struct i2c_device_id eeprom_id[] = {
    { "eeprom_demo", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, eeprom_id);

static struct i2c_driver eeprom_driver = {
    .driver = {
        .name = DRV_NAME,
    },
    .probe    = eeprom_probe,
    .remove   = eeprom_remove,
    .id_table = eeprom_id,
};
module_i2c_driver(eeprom_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sowmya");
MODULE_DESCRIPTION("Simple EEPROM I2C driver");

