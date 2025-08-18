// dummy_audio.c - Minimal ALSA PCM driver example
#include <linux/module.h>
#include <linux/init.h>
#include <sound/core.h>
#include <sound/pcm.h>

#define CARD_NAME "dummy_card"
#define DRIVER_NAME "dummy_audio"

static struct snd_card *card;
static struct snd_pcm *pcm;

static int dummy_pcm_open(struct snd_pcm_substream *substream)
{
    return 0; // nothing to init
}

static int dummy_pcm_close(struct snd_pcm_substream *substream)
{
    return 0;
}

static int dummy_pcm_hw_params(struct snd_pcm_substream *substream,
                               struct snd_pcm_hw_params *hw_params)
{
    return 0;
}

static int dummy_pcm_hw_free(struct snd_pcm_substream *substream)
{
    return 0;
}

static int dummy_pcm_prepare(struct snd_pcm_substream *substream)
{
    return 0;
}

static int dummy_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
    return 0;
}

static snd_pcm_uframes_t dummy_pcm_pointer(struct snd_pcm_substream *substream)
{
    return 0;
}

static const struct snd_pcm_ops dummy_pcm_ops = {
    .open      = dummy_pcm_open,
    .close     = dummy_pcm_close,
    .ioctl     = snd_pcm_lib_ioctl,
    .hw_params = dummy_pcm_hw_params,
    .hw_free   = dummy_pcm_hw_free,
    .prepare   = dummy_pcm_prepare,
    .trigger   = dummy_pcm_trigger,
    .pointer   = dummy_pcm_pointer,
};

static int __init dummy_init(void)
{
    int err;

    err = snd_card_new(NULL, -1, CARD_NAME, THIS_MODULE, 0, &card);
    if (err < 0)
        return err;

    err = snd_pcm_new(card, "Dummy PCM", 0, 1, 0, &pcm);
    if (err < 0) {
        snd_card_free(card);
        return err;
    }

    snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &dummy_pcm_ops);

    strcpy(card->driver, DRIVER_NAME);
    strcpy(card->shortname, "Dummy Sound Card");
    strcpy(card->longname, "Dummy ALSA Sound Card for Testing");

    err = snd_card_register(card);
    if (err < 0) {
        snd_card_free(card);
        return err;
    }

    pr_info("Dummy audio driver loaded\n");
    return 0;
}

static void __exit dummy_exit(void)
{
    snd_card_free(card);
    pr_info("Dummy audio driver unloaded\n");
}

module_init(dummy_init);
module_exit(dummy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT Example");
MODULE_DESCRIPTION("Minimal Dummy ALSA PCM Driver");

