// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/platform_device.h>
#include <sound/soc.h>

static struct snd_soc_dai_link_component cpu_dai = {
    .dai_name = "fe203000.i2s",   // Correct DAI name for RPi4 I2S
};

static struct snd_soc_dai_link_component codec_dai = {
    .name = "pcm512x.1-004d",     // Codec device name (I2C1 @ 0x4D)
    .dai_name = "pcm512x-hifi",   // Codec DAI
};

static struct snd_soc_dai_link_component platform_dai = {
    .name = "fe203000.i2s",
};

static struct snd_soc_dai_link dai_link = {
    .name = "Simple-I2S",
    .stream_name = "I2S Audio",
    .cpus = &cpu_dai,
    .num_cpus = 1,
    .codecs = &codec_dai,
    .num_codecs = 1,
    .platforms = &platform_dai,
    .num_platforms = 1,
    .dai_fmt = SND_SOC_DAIFMT_I2S |
           SND_SOC_DAIFMT_NB_NF |
           SND_SOC_DAIFMT_CBS_CFS, // CPU is master (BCLK & FS)

};

static struct snd_soc_card simple_card = {
    .name = "Simple I2S Audio Card",
    .owner = THIS_MODULE,
    .dai_link = &dai_link,
    .num_links = 1,
};

static int simple_i2s_probe(struct platform_device *pdev)
{
    simple_card.dev = &pdev->dev;
    dev_info(&pdev->dev, "Probing Simple I2S Audio Card...\n");
    return devm_snd_soc_register_card(&pdev->dev, &simple_card);
}

static struct platform_driver simple_i2s_driver = {
    .driver = {
        .name = "simple-i2s-card",
        .owner = THIS_MODULE,
    },
    .probe = simple_i2s_probe,
};

static struct platform_device *simple_i2s_device;

static int __init simple_i2s_init(void)
{
    int ret;

    // Create platform device
    simple_i2s_device = platform_device_register_simple("simple-i2s-card", -1, NULL, 0);
    if (IS_ERR(simple_i2s_device))
        return PTR_ERR(simple_i2s_device);

    // Register platform driver
    ret = platform_driver_register(&simple_i2s_driver);
    if (ret)
        platform_device_unregister(simple_i2s_device);

    pr_info("Simple I2S Audio driver initialized\n");
    return ret;
}

static void __exit simple_i2s_exit(void)
{
    platform_driver_unregister(&simple_i2s_driver);
    platform_device_unregister(simple_i2s_device);
    pr_info("Simple I2S Audio driver exited\n");
}

module_init(simple_i2s_init);
module_exit(simple_i2s_exit);

MODULE_AUTHOR("Kadali Harsha Sri Sampath");
MODULE_DESCRIPTION("Simple Audio Playback Driver Using I2S Interface");
MODULE_LICENSE("GPL");

