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
#include <linux/delay.h>

#include "pcm512x.h"

/* Debug prints */
#define DEBUG_PCM512X
#ifdef DEBUG_PCM512X
#define DBG(dev, fmt, args...) dev_info(dev, fmt, ##args)
#else
#define DBG(dev, fmt, args...)
#endif

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
	int vol_l;    /* last left volume */
	int vol_r;    /* last right volume */
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

static bool pcm512x_readable(struct device *dev, unsigned int reg)
{
	return reg < 0xff;
}

static bool pcm512x_volatile(struct device *dev, unsigned int reg)
{
	return false;
}

static const DECLARE_TLV_DB_SCALE(digital_tlv, -10350, 50, 1);

/* --------- SOFT VOLUME RAMP FUNCTION ---------- */
static void pcm512x_soft_set_volume(struct pcm512x_priv *pcm512x,
				    int new_l, int new_r)
{
	int old_l = pcm512x->vol_l;
	int old_r = pcm512x->vol_r;
	int step_l = (new_l > old_l) ? 1 : -1;
	int step_r = (new_r > old_r) ? 1 : -1;
	int v_l, v_r;

	DBG(NULL, "Ramping volume: L %d→%d  R %d→%d\n", old_l, new_l, old_r, new_r);

	for (v_l = old_l, v_r = old_r;
	     v_l != new_l || v_r != new_r;
	     v_l += (v_l != new_l ? step_l : 0),
	     v_r += (v_r != new_r ? step_r : 0)) {

		regmap_write(pcm512x->regmap, PCM512x_DIGITAL_VOLUME_2, v_l);
		regmap_write(pcm512x->regmap, PCM512x_DIGITAL_VOLUME_3, v_r);
		usleep_range(1000, 2000); /* ~1ms per step for smooth fade */
	}

	/* final value */
	regmap_write(pcm512x->regmap, PCM512x_DIGITAL_VOLUME_2, new_l);
	regmap_write(pcm512x->regmap, PCM512x_DIGITAL_VOLUME_3, new_r);

	pcm512x->vol_l = new_l;
	pcm512x->vol_r = new_r;
}

/* --------- Custom Volume Put Handler ---------- */
static int pcm512x_volume_put(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);
	int new_l = ucontrol->value.integer.value[0] & 0xFF;
	int new_r = ucontrol->value.integer.value[1] & 0xFF;

	mutex_lock(&pcm512x->mutex);

	pcm512x_soft_set_volume(pcm512x, new_l, new_r);

	mutex_unlock(&pcm512x->mutex);
	return 1; /* changed */
}

/* --------- Volume Control with custom PUT --------- */
static const struct snd_kcontrol_new pcm512x_controls[] = {
	SOC_DOUBLE_R_EXT_TLV("Digital Playback Volume",
			     PCM512x_DIGITAL_VOLUME_2,
			     PCM512x_DIGITAL_VOLUME_3,
			     0, 255, 1,
			     NULL,                       /* GET (default) */
			     pcm512x_volume_put,          /* Our soft PUT */
			     digital_tlv),

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

/* ---- Rest of the driver remains original ---- */

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

static int pcm512x_hw_params(struct snd_pcm_substream *substream,
			     struct snd_pcm_hw_params *params,
			     struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);
	int alen;
	int ret;

	switch (params_width(params)) {
	case 16: alen = PCM512x_ALEN_16; break;
	case 24: alen = PCM512x_ALEN_24; break;
	case 32: alen = PCM512x_ALEN_32; break;
	default:
		dev_err(component->dev, "Bad frame size: %d\n", params_width(params));
		return -EINVAL;
	}

	ret = regmap_update_bits(pcm512x->regmap, PCM512x_I2S_1, PCM512x_ALEN, alen);
	return ret;
}

static int pcm512x_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_component *component = dai->component;
	struct pcm512x_priv *pcm512x = snd_soc_component_get_drvdata(component);
	int afmt;

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:     afmt = PCM512x_AFMT_I2S; break;
	case SND_SOC_DAIFMT_RIGHT_J: afmt = PCM512x_AFMT_RTJ; break;
	case SND_SOC_DAIFMT_LEFT_J:  afmt = PCM512x_AFMT_LTJ; break;
	default:
		dev_err(component->dev, "unsupported DAI format\n");
		return -EINVAL;
	}

	return regmap_update_bits(pcm512x->regmap, PCM512x_I2S_1, PCM512x_AFMT, afmt);
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
			   SNDRV_PCM_FMTBIT_S32_LE
	},
	.ops = &pcm512x_dai_ops,
};

static int pcm512x_probe(struct device *dev, struct regmap *regmap)
{
	struct pcm512x_priv *pcm512x;
	int i, ret;

	pcm512x = devm_kzalloc(dev, sizeof(*pcm512x), GFP_KERNEL);
	if (!pcm512x)
		return -ENOMEM;

	mutex_init(&pcm512x->mutex);
	dev_set_drvdata(dev, pcm512x);
	pcm512x->regmap = regmap;

	/* Initial volume tracking */
	pcm512x->vol_l = 0x30;
	pcm512x->vol_r = 0x30;

	/* ... rest of original probe code unchanged ... */
	return 0;
}

MODULE_DESCRIPTION("ASoC PCM512x codec driver with soft volume ramp");
MODULE_AUTHOR("Original Author Mark Brown");
MODULE_LICENSE("GPL v2");

