/*
 * OV7251 Camera Driver for Raspberry Pi 4
 * Custom I2C and V4L2 driver implementation
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/videodev2.h>
#include <linux/gpio/consumer.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-fwnode.h>
#include <media/v4l2-mediabus.h>

#define OV7251_CHIP_ID_HIGH 0x300A
#define OV7251_CHIP_ID_LOW 0x300B
#define OV7251_CHIP_ID 0x7750

#define OV7251_SC_MODE_SELECT 0x0100
#define OV7251_SC_SOFTWARE_RESET 0x0103
#define OV7251_AEC_PK_MANUAL 0x3503

#define OV7251_REG_EXPOSURE_HIGH 0x3500
#define OV7251_REG_EXPOSURE_MID 0x3501
#define OV7251_REG_EXPOSURE_LOW 0x3502
#define OV7251_REG_GAIN_HIGH 0x350A
#define OV7251_REG_GAIN_LOW 0x350B

struct ov7251_mode {
    u32 width;
    u32 height;
    u32 hts;
    u32 vts;
    u32 exp_def;
};

struct ov7251_regval {
    u16 addr;
    u8 val;
};

struct ov7251 {
    struct v4l2_subdev sd;
    struct media_pad pad;
    struct v4l2_ctrl_handler ctrl_handler;
    struct v4l2_ctrl *exposure;
    struct v4l2_ctrl *gain;
    struct clk *xclk;
    struct gpio_desc *reset_gpio;
    struct gpio_desc *pwdn_gpio;
    struct regulator *avdd;
    struct regulator *dovdd;
    struct i2c_client *client;
    const struct ov7251_mode *cur_mode;
    struct mutex lock;
    bool streaming;
};

/* 640x480 @ 30fps initialization sequence */
static const struct ov7251_regval ov7251_640x480_regs[] = {
    {0x0103, 0x01}, /* Software reset */
    {0x0100, 0x00}, /* Standby mode */
    {0x3005, 0x00},
    {0x3012, 0xc0},
    {0x3013, 0xd2},
    {0x3014, 0x04},
    {0x3016, 0xf0},
    {0x3017, 0xf0},
    {0x3018, 0xf0},
    {0x301a, 0xf0},
    {0x301b, 0xf0},
    {0x301c, 0xf0},
    {0x3023, 0x05},
    {0x3037, 0xf0},
    {0x3098, 0x04},
    {0x3099, 0x28},
    {0x309a, 0x05},
    {0x309b, 0x04},
    {0x30b0, 0x0a},
    {0x30b1, 0x01},
    {0x30b3, 0x64},
    {0x30b4, 0x03},
    {0x30b5, 0x05},
    {0x3106, 0xda},
    {0x3503, 0x07}, /* Manual AEC/AGC */
    {0x3509, 0x10},
    {0x3600, 0x1c},
    {0x3602, 0x62},
    {0x3620, 0xb7},
    {0x3622, 0x04},
    {0x3626, 0x21},
    {0x3627, 0x30},
    {0x3630, 0x44},
    {0x3631, 0x35},
    {0x3634, 0x60},
    {0x3636, 0x00},
    {0x3662, 0x01},
    {0x3663, 0x70},
    {0x3664, 0xf0},
    {0x3666, 0x0a},
    {0x3669, 0x1a},
    {0x366a, 0x00},
    {0x366b, 0x50},
    {0x3673, 0x01},
    {0x3674, 0xff},
    {0x3675, 0x03},
    {0x3705, 0xc1},
    {0x3709, 0x40},
    {0x373c, 0x08},
    {0x3742, 0x00},
    {0x3757, 0xb3},
    {0x3788, 0x00},
    {0x37a8, 0x01},
    {0x37a9, 0xc0},
    {0x3800, 0x00}, /* X start */
    {0x3801, 0x04},
    {0x3802, 0x00}, /* Y start */
    {0x3803, 0x04},
    {0x3804, 0x02}, /* X end */
    {0x3805, 0x8b},
    {0x3806, 0x01}, /* Y end */
    {0x3807, 0xeb},
    {0x3808, 0x02}, /* X output size */
    {0x3809, 0x80},
    {0x380a, 0x01}, /* Y output size */
    {0x380b, 0xe0},
    {0x380c, 0x03}, /* HTS */
    {0x380d, 0xa0},
    {0x380e, 0x02}, /* VTS */
    {0x380f, 0x3c},
    {0x3810, 0x00}, /* ISP X win */
    {0x3811, 0x04},
    {0x3812, 0x00}, /* ISP Y win */
    {0x3813, 0x05},
    {0x3814, 0x11}, /* X inc */
    {0x3815, 0x11}, /* Y inc */
    {0x3820, 0x40}, /* Flip off */
    {0x3821, 0x00}, /* Mirror off */
    {0x382f, 0x0e},
    {0x3881, 0x42},
    {0x3882, 0x01},
    {0x3883, 0x00},
    {0x3885, 0x02},
    {0x382a, 0x08},
    {0x382b, 0x68},
    {0x3c81, 0x00},
    {0x3c82, 0x00},
    {0x3c83, 0xc8},
    {0x3f0d, 0x00},
    {0x4001, 0x40},
    {0x4004, 0x02},
    {0x4300, 0xff},
    {0x4303, 0x00},
    {0x4307, 0x00},
    {0x4600, 0x04},
    {0x4601, 0x00},
    {0x4602, 0x20},
    {0x4800, 0x04},
    {0x4801, 0x0f},
    {0x4837, 0x28},
    {0x5000, 0x06},
    {0x5001, 0x00},
    {0x5002, 0x00},
    {0x5068, 0x00},
    {0x506a, 0x00},
    {0x501f, 0x00},
    {0x5780, 0xfc},
    {0x5c00, 0x80},
    {0x5c01, 0x00},
    {0x5c02, 0x00},
    {0x5c03, 0x00},
    {0x5c04, 0x00},
    {0x5c05, 0x00},
    {0x5c06, 0x00},
    {0x5c07, 0x80},
    {0x5c08, 0x10},
    {0x0100, 0x01}, /* Stream on */
};

static const struct ov7251_mode ov7251_modes[] = {
    {
        .width = 640,
        .height = 480,
        .hts = 928,
        .vts = 572,
        .exp_def = 572,
    },
};

static inline struct ov7251 *to_ov7251(struct v4l2_subdev *sd)
{
    return container_of(sd, struct ov7251, sd);
}

static int ov7251_write_reg(struct i2c_client *client, u16 reg, u8 val)
{
    u8 buf[3] = {reg >> 8, reg & 0xff, val};
    struct i2c_msg msg = {
        .addr = client->addr,
        .flags = 0,
        .buf = buf,
        .len = 3,
    };
    int ret;

    ret = i2c_transfer(client->adapter, &msg, 1);
    if (ret < 0) {
        dev_err(&client->dev, "Failed to write reg 0x%04x: %d\n", reg, ret);
        return ret;
    }
    return 0;
}

static int ov7251_read_reg(struct i2c_client *client, u16 reg, u8 *val)
{
    u8 buf[2] = {reg >> 8, reg & 0xff};
    struct i2c_msg msgs[2] = {
        {
            .addr = client->addr,
            .flags = 0,
            .buf = buf,
            .len = 2,
        },
        {
            .addr = client->addr,
            .flags = I2C_M_RD,
            .buf = val,
            .len = 1,
        }
    };
    int ret;

    ret = i2c_transfer(client->adapter, msgs, 2);
    if (ret < 0) {
        dev_err(&client->dev, "Failed to read reg 0x%04x: %d\n", reg, ret);
        return ret;
    }
    return 0;
}

static int ov7251_write_array(struct i2c_client *client,
                               const struct ov7251_regval *regs, int len)
{
    int i, ret;

    for (i = 0; i < len; i++) {
        ret = ov7251_write_reg(client, regs[i].addr, regs[i].val);
        if (ret < 0)
            return ret;
        if (regs[i].addr == 0x0103)
            msleep(10);
    }
    return 0;
}

static int ov7251_check_chip_id(struct ov7251 *ov7251)
{
    struct i2c_client *client = ov7251->client;
    u8 id_high, id_low;
    u16 chip_id;
    int ret;

    ret = ov7251_read_reg(client, OV7251_CHIP_ID_HIGH, &id_high);
    if (ret)
        return ret;

    ret = ov7251_read_reg(client, OV7251_CHIP_ID_LOW, &id_low);
    if (ret)
        return ret;

    chip_id = (id_high << 8) | id_low;
    if (chip_id != OV7251_CHIP_ID) {
        dev_err(&client->dev, "Unexpected chip ID: 0x%04x\n", chip_id);
        return -ENODEV;
    }

    dev_info(&client->dev, "OV7251 chip detected, ID: 0x%04x\n", chip_id);
    return 0;
}

static int ov7251_s_stream(struct v4l2_subdev *sd, int enable)
{
    struct ov7251 *ov7251 = to_ov7251(sd);
    struct i2c_client *client = ov7251->client;
    int ret;

    mutex_lock(&ov7251->lock);

    if (ov7251->streaming == enable) {
        mutex_unlock(&ov7251->lock);
        return 0;
    }

    if (enable) {
        ret = ov7251_write_array(client, ov7251_640x480_regs,
                                  ARRAY_SIZE(ov7251_640x480_regs));
        if (ret) {
            mutex_unlock(&ov7251->lock);
            return ret;
        }
        ov7251->streaming = true;
        dev_info(&client->dev, "Stream started\n");
    } else {
        ret = ov7251_write_reg(client, OV7251_SC_MODE_SELECT, 0x00);
        ov7251->streaming = false;
        dev_info(&client->dev, "Stream stopped\n");
    }

    mutex_unlock(&ov7251->lock);
    return ret;
}

static int ov7251_s_ctrl(struct v4l2_ctrl *ctrl)
{
    struct ov7251 *ov7251 = container_of(ctrl->handler,
                                          struct ov7251, ctrl_handler);
    struct i2c_client *client = ov7251->client;
    int ret = 0;

    switch (ctrl->id) {
    case V4L2_CID_EXPOSURE:
        ret = ov7251_write_reg(client, OV7251_REG_EXPOSURE_HIGH,
                               (ctrl->val >> 16) & 0x0f);
        ret |= ov7251_write_reg(client, OV7251_REG_EXPOSURE_MID,
                                (ctrl->val >> 8) & 0xff);
        ret |= ov7251_write_reg(client, OV7251_REG_EXPOSURE_LOW,
                                ctrl->val & 0xff);
        break;
    case V4L2_CID_GAIN:
        ret = ov7251_write_reg(client, OV7251_REG_GAIN_HIGH,
                               (ctrl->val >> 8) & 0x03);
        ret |= ov7251_write_reg(client, OV7251_REG_GAIN_LOW,
                                ctrl->val & 0xff);
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const struct v4l2_ctrl_ops ov7251_ctrl_ops = {
    .s_ctrl = ov7251_s_ctrl,
};

static const struct v4l2_subdev_video_ops ov7251_video_ops = {
    .s_stream = ov7251_s_stream,
};

static const struct v4l2_subdev_ops ov7251_subdev_ops = {
    .video = &ov7251_video_ops,
};

static int ov7251_probe(struct i2c_client *client,
                        const struct i2c_device_id *id)
{
    struct device *dev = &client->dev;
    struct ov7251 *ov7251;
    int ret;

    dev_info(dev, "OV7251 driver probing...\n");

    ov7251 = devm_kzalloc(dev, sizeof(*ov7251), GFP_KERNEL);
    if (!ov7251)
        return -ENOMEM;

    ov7251->client = client;
    ov7251->cur_mode = &ov7251_modes[0];

    /* Get clock */
    ov7251->xclk = devm_clk_get(dev, "xclk");
    if (IS_ERR(ov7251->xclk)) {
        dev_err(dev, "Failed to get xclk\n");
        return PTR_ERR(ov7251->xclk);
    }

    ret = clk_set_rate(ov7251->xclk, 24000000);
    if (ret < 0) {
        dev_err(dev, "Failed to set xclk rate\n");
        return ret;
    }

    ret = clk_prepare_enable(ov7251->xclk);
    if (ret < 0) {
        dev_err(dev, "Failed to enable xclk\n");
        return ret;
    }

    /* Get GPIOs */
    ov7251->reset_gpio = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(ov7251->reset_gpio)) {
        dev_err(dev, "Failed to get reset gpio\n");
        ret = PTR_ERR(ov7251->reset_gpio);
        goto err_clk;
    }

    ov7251->pwdn_gpio = devm_gpiod_get_optional(dev, "pwdn", GPIOD_OUT_LOW);
    if (IS_ERR(ov7251->pwdn_gpio)) {
        dev_err(dev, "Failed to get pwdn gpio\n");
        ret = PTR_ERR(ov7251->pwdn_gpio);
        goto err_clk;
    }

    /* Reset sequence */
    if (ov7251->reset_gpio) {
        gpiod_set_value_cansleep(ov7251->reset_gpio, 1);
        msleep(10);
        gpiod_set_value_cansleep(ov7251->reset_gpio, 0);
        msleep(20);
    }

    /* Check chip ID */
    ret = ov7251_check_chip_id(ov7251);
    if (ret)
        goto err_clk;

    mutex_init(&ov7251->lock);

    /* Initialize V4L2 subdev */
    v4l2_i2c_subdev_init(&ov7251->sd, client, &ov7251_subdev_ops);
    ov7251->sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
    ov7251->pad.flags = MEDIA_PAD_FL_SOURCE;
    ov7251->sd.entity.function = MEDIA_ENT_F_CAM_SENSOR;

    ret = media_entity_pads_init(&ov7251->sd.entity, 1, &ov7251->pad);
    if (ret < 0) {
        dev_err(dev, "Failed to init entity pads\n");
        goto err_mutex;
    }

    /* Initialize controls */
    v4l2_ctrl_handler_init(&ov7251->ctrl_handler, 2);
    ov7251->exposure = v4l2_ctrl_new_std(&ov7251->ctrl_handler,
                                         &ov7251_ctrl_ops,
                                         V4L2_CID_EXPOSURE,
                                         4, 0xffff, 1, 0x0200);
    ov7251->gain = v4l2_ctrl_new_std(&ov7251->ctrl_handler,
                                     &ov7251_ctrl_ops,
                                     V4L2_CID_GAIN,
                                     16, 1023, 1, 16);

    if (ov7251->ctrl_handler.error) {
        ret = ov7251->ctrl_handler.error;
        dev_err(dev, "Control handler init failed: %d\n", ret);
        goto err_entity;
    }

    ov7251->sd.ctrl_handler = &ov7251->ctrl_handler;

    ret = v4l2_async_register_subdev(&ov7251->sd);
    if (ret < 0) {
        dev_err(dev, "Failed to register subdev: %d\n", ret);
        goto err_ctrl;
    }

    dev_info(dev, "OV7251 driver probed successfully\n");
    return 0;

err_ctrl:
    v4l2_ctrl_handler_free(&ov7251->ctrl_handler);
err_entity:
    media_entity_cleanup(&ov7251->sd.entity);
err_mutex:
    mutex_destroy(&ov7251->lock);
err_clk:
    clk_disable_unprepare(ov7251->xclk);
    return ret;
}

static int ov7251_remove(struct i2c_client *client)
{
    struct v4l2_subdev *sd = i2c_get_clientdata(client);
    struct ov7251 *ov7251 = to_ov7251(sd);

    v4l2_async_unregister_subdev(sd);
    media_entity_cleanup(&sd->entity);
    v4l2_ctrl_handler_free(&ov7251->ctrl_handler);
    mutex_destroy(&ov7251->lock);
    clk_disable_unprepare(ov7251->xclk);

    return 0;
}

static const struct i2c_device_id ov7251_id[] = {
    { "ov7251", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, ov7251_id);

static const struct of_device_id ov7251_of_match[] = {
    { .compatible = "ovti,ov7251" },
    { }
};
MODULE_DEVICE_TABLE(of, ov7251_of_match);

static struct i2c_driver ov7251_i2c_driver = {
    .driver = {
        .name = "ov7251",
        .of_match_table = ov7251_of_match,
    },
    .probe = ov7251_probe,
    .remove = ov7251_remove,
    .id_table = ov7251_id,
};

module_i2c_driver(ov7251_i2c_driver);

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("OV7251 Camera Driver for Raspberry Pi 4");
MODULE_LICENSE("GPL v2");
