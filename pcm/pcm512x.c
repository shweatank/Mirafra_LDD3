/*
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Minimal Driver for the PCM512x CODECs
 * Simplified version with basic playback functionality only
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/pcm_params.h>
#include <sound/tlv.h>
#include "pcm512x.h"

#define PCM512x_NUM_SUPPLIES 3

static const char * const pcm512x_supply_names[PCM512x_NUM_SUPPLIES] = {
    "AVDD",
    "DVDD",
    "CPVDD",
};

struct pcm512x_priv {
    struct regmap *regmap;
    struct clk *sclk;
    struct regulator_bulk_data supplies[PCM512x_NUM_SUPPLIES];
    int fmt;
    int mute;
    struct mutex mutex;
};

static const struct reg_default pcm512x_reg_defaults[] = {
    { PCM512x_RESET,             0x00 },
    { PCM512x_POWER,             0x00 },
    { PCM512x_MUTE,              0x00 },
    { PCM512x_DSP,               0x00 },
    { PCM512x_DAC_ROUTING,       0x11 },
    { PCM512x_DIGITAL_VOLUME_2,  0x30 },
    { PCM512x_DIGITAL_VOLUME_3,  0x30 },
    { PCM512x_DIGITAL_MUTE_1,    0x22 },
    { PCM512x_ANALOG_GAIN_CTRL,  0x00 },
    { PCM512x_I2S_1,             0x02 },
};

// Define pcm512x_ranges as an array of struct regmap_range_cfg
static const struct regmap_range_cfg pcm512x_ranges[] = {
    {
        .name = "Page 0",
        .range_min = 0,
        .range_max = 0xFF,
        .selector_reg = 0x00,
        .selector_mask = 0xFF,
        .window_start = 0,
        .window_len = 0x100,
    },
    // Add more pages if needed
};

static bool pcm512x_readable(struct device *dev, unsigned int reg)
{
    switch (reg) {
    case PCM512x_RESET:
    case PCM512x_POWER:
    case PCM512x_MUTE:
    case PCM512x_DSP:
    case PCM512x_I2S_1:
    case PCM512x_I2S_2:
    case PCM512x_DAC_ROUTING:
    case PCM512x_DIGITAL_VOLUME_2:
    case PCM512x_DIGITAL_VOLUME_3:
    case PCM512x_DIGITAL_MUTE_1:
    case PCM512x_ANALOG_GAIN_CTRL:
        return true;
    default:
        return reg < PCM512x_MAX_REGISTER;
    }
}

static bool pcm512x_volatile(struct device *dev, unsigned int reg)
{
    return false;
}

static const DECLARE_TLV_DB_SCALE(digital_tlv, -10350, 50, 1);

static int pcm512x_update_mute(struct pcm512x_priv *pcm512x)
{
    return regmap_update_bits(pcm512x->regmap, PCM512x_MUTE,
                  PCM512x_RQML | PCM512x_RQMR,
                  (pcm512x->mute ? PCM512x_RQML | PCM512x_RQMR : 0));
}

static int pcm512x_configure_pll(struct snd_soc_component *component, unsigned int rate)
{
    struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);
    int ret;

    if (rate == 44100) {
        dev_info(component->dev, "Configuring PLL for 44.1 kHz\n");

        // Set PLL registers for 44.1 kHz (assuming SCK = 12.288 MHz)
        ret = regmap_write(pcm512x->regmap, 0x00, 0x00); // Select Page 0
        ret |= regmap_write(pcm512x->regmap, 0x14, 0x00); // PLL P = 1, R = 1
        ret |= regmap_write(pcm512x->regmap, 0x15, 0x07); // PLL J = 7
        ret |= regmap_write(pcm512x->regmap, 0x16, 0x14); // PLL D (MSB) = 0x14
        ret |= regmap_write(pcm512x->regmap, 0x17, 0x90); // PLL D (LSB) = 0x90
        ret |= regmap_update_bits(pcm512x->regmap, 0x04, 0x01, 0x01); // Enable PLL

        if (ret != 0) {
            dev_err(component->dev, "Failed to configure PLL: %d\n", ret);
            return ret;
        }
    }

    return 0;
}

static int pcm512x_digital_playback_switch_get(struct snd_kcontrol *kcontrol,
                                             struct snd_ctl_elem_value *ucontrol)
{
    struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
    struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);

    mutex_lock(&pcm512x->mutex);
    ucontrol->value.integer.value[0] = !pcm512x->mute;
    ucontrol->value.integer.value[1] = !pcm512x->mute;
    mutex_unlock(&pcm512x->mutex);
    return 0;
}

static int pcm512x_digital_playback_switch_put(struct snd_kcontrol *kcontrol,
                                             struct snd_ctl_elem_value *ucontrol)
{
    struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
    struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);
    int ret, changed = 0;
    int new_mute = !(ucontrol->value.integer.value[0]);

    mutex_lock(&pcm512x->mutex);
    if (pcm512x->mute != new_mute) {
        pcm512x->mute = new_mute;
        changed = 1;
        ret = pcm512x_update_mute(pcm512x);
        if (ret != 0) {
            dev_err(component->dev, "Failed to update digital mute: %d\n", ret);
            mutex_unlock(&pcm512x->mutex);
            return ret;
        }
    }
    mutex_unlock(&pcm512x->mutex);
    return changed;
}

static const struct snd_kcontrol_new pcm512x_controls[] = {
    SOC_DOUBLE_R_TLV("Digital Playback Volume", PCM512x_DIGITAL_VOLUME_2,
                     PCM512x_DIGITAL_VOLUME_3, 0, 255, 1, digital_tlv),
    {
        .iface = SNDRV_CTL_ELEM_IFACE_MIXER,
        .name = "Digital Playback Switch",
        .index = 0,
        .access = SNDRV_CTL_ELEM_ACCESS_READWRITE,
        .info = snd_ctl_boolean_stereo_info,
        .get = pcm512x_digital_playback_switch_get,
        .put = pcm512x_digital_playback_switch_put
    },
};

static const struct snd_soc_dapm_widget pcm512x_dapm_widgets[] = {
    SND_SOC_DAPM_DAC("DACL", NULL, SND_SOC_NOPM, 0, 0),
    SND_SOC_DAPM_DAC("DACR", NULL, SND_SOC_NOPM, 0, 0),
    SND_SOC_DAPM_OUTPUT("OUTL"),
    SND_SOC_DAPM_OUTPUT("OUTR"),
};

static const struct snd_soc_dapm_route pcm512x_dapm_routes[] = {
    { "DACL", NULL, "Playback" },
    { "DACR", NULL, "Playback" },
    { "OUTL", NULL, "DACL" },
    { "OUTR", NULL, "DACR" },
};

static int pcm512x_set_bias_level(struct snd_soc_component *component,
                                  enum snd_soc_bias_level level)
{
    struct pcm512x_priv *pcm512x = dev_get_drvdata(component->dev);
    int ret;

    switch (level) {
    case SND_SOC_BIAS_STANDBY:
        ret = regmap_update_bits(pcm512x->regmap, PCM512x_POWER,
                                  PCM512x_RQST, 0);
        if (ret != 0) {
            dev_err(component->dev, "Failed to remove standby: %d\n", ret);
            return ret;
        }
        break;
    case SND_SOC_BIAS_OFF:
        ret = regmap_update_bits(pcm512x->regmap, PCM512x_POWER,
                                  PCM512x_RQST, PCM512x_RQST);
        if (ret != 0) {
            dev_err(component->dev, "Failed to request standby: %d\n", ret);
            return ret;
        }
        break;
    default:
        break;
    }
    return 0;
}

static int pcm512x_hw_params(struct snd_pcm_substream *substream,
                             struct snd_pcm_hw_params *params,
                             struct snd_soc_dai *dai)
{
    struct snd_soc_component *component = dai->component;
    struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);
    int alen, ret;
    unsigned int rate = params_rate(params);

    // Configure PLL for the given sample rate
    ret = pcm512x_configure_pll(component, rate);
    if (ret < 0) {
        dev_err(component->dev, "Failed to configure PLL: %d\n", ret);
        return ret;
    }

    switch (params_width(params)) {
    case 16:
        alen = PCM512x_ALEN_32;
        break;
    case 24:
        alen = PCM512x_ALEN_24;
        break;
    case 32:
        alen = PCM512x_ALEN_32;
        break;
    default:
        dev_err(component->dev, "Unsupported sample width %d\n", params_width(params));
        return -EINVAL;
    }

    ret = regmap_update_bits(pcm512x->regmap, PCM512x_I2S_1, PCM512x_ALEN, alen);
    if (ret != 0) {
        dev_err(component->dev, "Failed to set frame size: %d\n", ret);
        return ret;
    }

    dev_info(component->dev, "PCM512x: width=%d -> ALEN=0x%x ret=%d\n",
             params_width(params), alen, ret);
    return 0;
}

static int pcm512x_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
    struct snd_soc_component *component = dai->component;
    struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);
    int afmt, ret;

    switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
    case SND_SOC_DAIFMT_I2S:
        afmt = PCM512x_AFMT_I2S;
        break;
    case SND_SOC_DAIFMT_RIGHT_J:
        afmt = PCM512x_AFMT_RTJ;
        break;
    case SND_SOC_DAIFMT_LEFT_J:
        afmt = PCM512x_AFMT_LTJ;
        break;
    default:
        dev_err(component->dev, "Unsupported DAI format (bus)\n");
        return -EINVAL;
    }

    ret = regmap_update_bits(pcm512x->regmap, PCM512x_I2S_1, PCM512x_AFMT, afmt);
    if (ret != 0) {
        dev_err(component->dev, "Failed to set data format: %d\n", ret);
        return ret;
    }

    dev_info(component->dev, "PCM512x: fmt=0x%x AFMT=0x%x ret=%d\n",
             fmt, afmt, ret);
    pcm512x->fmt = fmt;
    return 0;
}

static int pcm512x_set_sysclk(struct snd_soc_dai *dai,
                              int clk_id, unsigned int freq, int dir)
{
    struct snd_soc_component *component = dai->component;
    struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);
    int ret;

    dev_info(component->dev, "pcm512x_set_sysclk: id=%d freq=%u dir=%d\n",
             clk_id, freq, dir);

    if (dir == SND_SOC_CLOCK_IN) {
        // Use external SCK (4-wire mode)
        ret = regmap_update_bits(pcm512x->regmap, 0x0D, 0x70, 0x00); // SRCREF = SCK
    } else {
        // Use BCK as PLL reference (3-wire mode)
        ret = regmap_update_bits(pcm512x->regmap, 0x0D, 0x70, 0x10); // SRCREF = BCK
    }

    if (ret != 0) {
        dev_err(component->dev, "Failed to set clock source: %d\n", ret);
        return ret;
    }

    return 0;
}

static int pcm512x_mute(struct snd_soc_dai *dai, int mute, int direction)
{
    struct snd_soc_component *component = dai->component;
    struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);
    int ret;

    mutex_lock(&pcm512x->mutex);
    pcm512x->mute = mute;
    ret = pcm512x_update_mute(pcm512x);
    mutex_unlock(&pcm512x->mutex);
    return ret;
}

static const struct snd_soc_dai_ops pcm512x_dai_ops = {
    .hw_params = pcm512x_hw_params,
    .set_fmt = pcm512x_set_fmt,
    .mute_stream = pcm512x_mute,
    .no_capture_mute = 1,
    .set_sysclk = pcm512x_set_sysclk,
};

static struct snd_soc_dai_driver pcm512x_dai = {
    .name = "pcm512x-hifi",
    .playback = {
        .stream_name = "Playback",
        .channels_min = 2,
        .channels_max = 2,
        .rates = SNDRV_PCM_RATE_8000_192000,
        .formats = SNDRV_PCM_FMTBIT_S16_LE |
                   SNDRV_PCM_FMTBIT_S24_LE |
                   SNDRV_PCM_FMTBIT_S32_LE,
    },
    .ops = &pcm512x_dai_ops,
};

static const struct snd_soc_component_driver pcm512x_component_driver = {
    .set_bias_level     = pcm512x_set_bias_level,
    .controls           = pcm512x_controls,
    .num_controls       = ARRAY_SIZE(pcm512x_controls),
    .dapm_widgets       = pcm512x_dapm_widgets,
    .num_dapm_widgets   = ARRAY_SIZE(pcm512x_dapm_widgets),
    .dapm_routes        = pcm512x_dapm_routes,
    .num_dapm_routes    = ARRAY_SIZE(pcm512x_dapm_routes),
    .use_pmdown_time    = 1,
    .endianness         = 1,
};

// Define regmap_config using pcm512x_ranges
const struct regmap_config pcm512x_regmap = {
    .reg_bits = 8,
    .val_bits = 8,
    .readable_reg = pcm512x_readable,
    .volatile_reg = pcm512x_volatile,
    .ranges = pcm512x_ranges,
    .num_ranges = ARRAY_SIZE(pcm512x_ranges),
    .max_register = PCM512x_MAX_REGISTER,
    .reg_defaults = pcm512x_reg_defaults,
    .num_reg_defaults = ARRAY_SIZE(pcm512x_reg_defaults),
    .cache_type = REGCACHE_RBTREE,
};
EXPORT_SYMBOL_GPL(pcm512x_regmap);

int pcm512x_probe(struct device *dev, struct regmap *regmap)
{
    struct pcm512x_priv *pcm512x;
    int i, ret;

    pcm512x = devm_kzalloc(dev, sizeof(struct pcm512x_priv), GFP_KERNEL);
    if (!pcm512x)
        return -ENOMEM;

    mutex_init(&pcm512x->mutex);
    dev_set_drvdata(dev, pcm512x);
    pcm512x->regmap = regmap;

    for (i = 0; i < ARRAY_SIZE(pcm512x->supplies); i++)
        pcm512x->supplies[i].supply = pcm512x_supply_names[i];

    ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(pcm512x->supplies),
                                  pcm512x->supplies);
    if (ret != 0) {
        dev_err(dev, "Failed to get supplies: %d\n", ret);
        return ret;
    }

    ret = regulator_bulk_enable(ARRAY_SIZE(pcm512x->supplies),
                                pcm512x->supplies);
    if (ret != 0) {
        dev_err(dev, "Failed to enable supplies: %d\n", ret);
        return ret;
    }

    ret = regmap_write(regmap, PCM512x_RESET, PCM512x_RSTM | PCM512x_RSTR);
    if (ret != 0) {
        dev_err(dev, "Failed to reset device (set): %d\n", ret);
        goto err;
    }

    ret = regmap_write(regmap, PCM512x_RESET, 0);
    if (ret != 0) {
        dev_err(dev, "Failed to reset device (clear): %d\n", ret);
        goto err;
    }

    pcm512x->sclk = devm_clk_get(dev, NULL);
    if (PTR_ERR(pcm512x->sclk) == -EPROBE_DEFER) {
        ret = -EPROBE_DEFER;
        goto err;
    }

    if (!IS_ERR(pcm512x->sclk)) {
        ret = clk_prepare_enable(pcm512x->sclk);
        if (ret != 0) {
            dev_err(dev, "Failed to enable SCLK: %d\n", ret);
            goto err;
        }
    }

    ret = regmap_update_bits(pcm512x->regmap, PCM512x_POWER,
                             PCM512x_RQST, PCM512x_RQST);
    if (ret != 0) {
        dev_err(dev, "Failed to request standby: %d\n", ret);
        goto err_clk;
    }

    pm_runtime_set_active(dev);
    pm_runtime_enable(dev);
    pm_runtime_idle(dev);

    ret = devm_snd_soc_register_component(dev, &pcm512x_component_driver,
                                          &pcm512x_dai, 1);
    if (ret != 0) {
        dev_err(dev, "Failed to register CODEC: %d\n", ret);
        goto err_pm;
    }

    return 0;

err_pm:
    pm_runtime_disable(dev);
err_clk:
    if (!IS_ERR(pcm512x->sclk))
        clk_disable_unprepare(pcm512x->sclk);
err:
    regulator_bulk_disable(ARRAY_SIZE(pcm512x->supplies), pcm512x->supplies);
    return ret;
}
EXPORT_SYMBOL_GPL(pcm512x_probe);

void pcm512x_remove(struct device *dev)
{
    struct pcm512x_priv *pcm512x = dev_get_drvdata(dev);

    pm_runtime_disable(dev);
    if (!IS_ERR(pcm512x->sclk))
        clk_disable_unprepare(pcm512x->sclk);
    regulator_bulk_disable(ARRAY_SIZE(pcm512x->supplies), pcm512x->supplies);
}
EXPORT_SYMBOL_GPL(pcm512x_remove);

#ifdef CONFIG_PM
static int pcm512x_suspend(struct device *dev)
{
    struct pcm512x_priv *pcm512x = dev_get_drvdata(dev);
    int ret;

    ret = regmap_update_bits(pcm512x->regmap, PCM512x_POWER,
                             PCM512x_RQPD, PCM512x_RQPD);
    if (ret != 0)
        return ret;

    regulator_bulk_disable(ARRAY_SIZE(pcm512x->supplies), pcm512x->supplies);
    if (!IS_ERR(pcm512x->sclk))
        clk_disable_unprepare(pcm512x->sclk);

    return 0;
}

static int pcm512x_resume(struct device *dev)
{
    struct pcm512x_priv *pcm512x = dev_get_drvdata(dev);
    int ret;

    if (!IS_ERR(pcm512x->sclk)) {
        ret = clk_prepare_enable(pcm512x->sclk);
        if (ret != 0)
            return ret;
    }

    ret = regulator_bulk_enable(ARRAY_SIZE(pcm512x->supplies),
                                pcm512x->supplies);
    if (ret != 0)
        return ret;

    ret = regcache_sync(pcm512x->regmap);
    if (ret != 0)
        return ret;

    ret = regmap_update_bits(pcm512x->regmap, PCM512x_POWER,
                             PCM512x_RQPD, 0);
    return ret;
}
#endif

const struct dev_pm_ops pcm512x_pm_ops = {
    SET_RUNTIME_PM_OPS(pcm512x_suspend, pcm512x_resume, NULL)
};
EXPORT_SYMBOL_GPL(pcm512x_pm_ops);

MODULE_DESCRIPTION("ASoC PCM512x codec driver - Minimal Version");
MODULE_AUTHOR("<Pavan>");
MODULE_LICENSE("GPL v2");

