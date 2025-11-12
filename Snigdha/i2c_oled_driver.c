#include<linux/module.h>
#include<linux/i2c.h>
#include<linux/of.h>
#include<linux/delay.h>

#define SSD1306_CMD 0x00
#define SSD1306_DATA 0x40


static void ssd1306_write_command(struct i2c_client *client, u8 cmd)
{
	u8 buffer[2]= {SSD1306_CMD, cmd};
	i2c_master_send(client,buffer,2);
}


static void ssd1306_write_data(struct i2c_client *client, u8 data)
{
	u8 buffer[2]= { SSD1306_DATA, data};
	i2c_master_send)client,buffer,2);
}

static void ssd1306_init_display(struct i2c_client *client)
{
	msleep(100);
	
	ssd1306_write_command(client, 0xAE);
	ssd1306_write_command(client, 0xA8);
	ssd1306_write_command(client, 0x3F);
	ssd1306_write_command(client, 0xD3);
	ssd1306_write_command(client, 0x00);
	ssd1306_write_command(client, 0x40 | 0x0);
	ssd1306_write_command(client, 0xA1);
	ssd1306_write_command(client, 0xC8);
	ssd1306_write_command(client, 0xDA);
	ssd1306_write_command(client, 0x12);
	ssd1306_write_command(client, 0x81);
	ssd1306_write_command(client, 0x7F);
	ssd1306_write_command(client, 0xD9);
	ssd1306_write_command(client, 0xF1);
	ssd1306_write_command(client, 0xDB);
	ssd1306_write_command(client, 0x40);
	ssd1306_write_command(client, 0xD5);
	ssd1306_write_command(client, 0x80);
	ssd1306_write_command(client, 0x20);
	ssd1306_write_command(client, 0x00);
	ssd1306_write_command(client, 0xA4);
	ssd1306_write_command(client, 0xA6);
	ssd1306_write_command(client, 0xAF);
}

/*static void ssd1306_init_display(struct i2c_client *client)
{
	msleep(100);
	ssd1306_write_command(client, 0xAE);
	ssd1306_write_command(client, 0xA4);
	ssd1306_write_command(client, 0xAF);
}*/

static int ssd1306_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	dev_info(&client->dev, "SSD1306 OLED device detected\n");
	ssd1306_init_display(client);
	return 0;
}

static void ssd1306_remove(struct i2c_client *client)
{
	dev_info(&client->dev, "SSD1306 OLED reoved\n");
}

static const struct of_device_id ssd1306_dt_ids[] = {
	{ .compatible = "my_i2c,ssd1306" },
	{}
};

MODULE_DEVICE_TABLE(of, ssd1306_dt_ids);

static struct i2c_driver ssd1306_driver = {
	.driver = {
		.name = "ssd1306_platform",
		.of_match_table = ssd1306_dt_ids,
	},
	.probe = ssd1306_probe,
	.remove = ssd1306_remove,

};

module_i2c_driver(ssd1306_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snigdha");
MODULE_DESCRIPTION("SSD1306 OLED I2C Platform Driver");

