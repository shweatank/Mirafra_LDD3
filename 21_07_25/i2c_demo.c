#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/init.h>

#define  DRV_NAME "i2c_demo"
#define SLAVE_ADDR 0x31

static int i2c_demo_probe(struct i2c_client *client)
{
    char tx[2] = {0x01, 0xAB}; // Write 0xAB to register 0x01
    char reg = 0x01;
    char rx;

    pr_info("i2c_demo: Probed device at 0x%02x\n", client->addr);

    i2c_master_send(client, tx, 2);
    i2c_master_send(client, &reg, 1);
    i2c_master_recv(client, &rx, 1);

    pr_info("i2c_demo: Read back 0x%02x from reg 0x01\n", rx);

    return 0;
}

static void i2c_demo_remove(struct i2c_client *client)
{
    pr_info("i2c_demo: Removed device\n");
}

static const struct i2c_device_id i2c_demo_id[] = {
    { "i2c_demo", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, i2c_demo_id);

static struct i2c_driver i2c_demo_driver = {
    .driver = {
        .name = DRV_NAME,
    },
    .probe = i2c_demo_probe,
    .remove = i2c_demo_remove,
    .id_table = i2c_demo_id,
};
module_i2c_driver(i2c_demo_driver);

/*static int __init i2c_demo_init(void)
{
    return i2c_add_driver(&i2c_demo_driver);
}

static void __exit i2c_demo_exit(void)
{
    i2c_del_driver(&i2c_demo_driver);
}

module_init(i2c_demo_init);
module_exit(i2c_demo_exit);*/

//module_i2c_driver(i2c_demo_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Simple I2C driver");

