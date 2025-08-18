#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/sound.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>

#define CARD_NAME       "MyAudioCard"
#define DRIVER_NAME     "my_audio"
#define PCM_DEVICES     1
#define PCM_SUBSTREAMS  1

static struct snd_card *my_card;
static struct snd_pcm *my_pcm;

static int my_pcm_open(struct snd_pcm_substream *substream)
{
    struct snd_pcm_runtime *runtime = substream->runtime;

    runtime->hw = (struct snd_pcm_hardware) {
       	.info = SNDRV_PCM_INFO_INTERLEAVED,
        .formats = SNDRV_PCM_FMTBIT_S16_LE,
        .rates = SNDRV_PCM_RATE_44100,
        .rate_min = 44100,
        .rate_max = 44100,
        .channels_min = 2,
        .channels_max = 2,
        .buffer_bytes_max = 64 * 1024,
        .period_bytes_min = 64,
        .period_bytes_max = 64 * 1024,
        .periods_min = 1,
        .periods_max = 1024,
    };

    return 0;
}

static int my_pcm_close(struct snd_pcm_substream *substream)
{
    return 0;
}

static int my_pcm_hw_params(struct snd_pcm_substream *substream,
                            struct snd_pcm_hw_params *hw_params)
{
    return snd_pcm_lib_malloc_pages(substream,
                                    params_buffer_bytes(hw_params));
}

static int my_pcm_hw_free(struct snd_pcm_substream *substream)
{
    return snd_pcm_lib_free_pages(substream);
}

static int my_pcm_prepare(struct snd_pcm_substream *substream)
{
    return 0;
}

static int my_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
    switch (cmd) {
    case SNDRV_PCM_TRIGGER_START:
    case SNDRV_PCM_TRIGGER_STOP:
        return 0;
    default:
        return -EINVAL;
    }
}

static snd_pcm_uframes_t my_pcm_pointer(struct snd_pcm_substream *substream)
{
    return 0;
}

static const struct snd_pcm_ops my_pcm_ops = {
    .open      = my_pcm_open,
    .close     = my_pcm_close,
    .ioctl     = snd_pcm_lib_ioctl,
    .hw_params = my_pcm_hw_params,
    .hw_free   = my_pcm_hw_free,
    .prepare   = my_pcm_prepare,
    .trigger   = my_pcm_trigger,
    .pointer   = my_pcm_pointer,
};

static int __init my_audio_init(void)
{
    int err;

    // Create a new sound card instance
    err = snd_card_new(NULL, -1, DRIVER_NAME, THIS_MODULE, 0, &my_card);
    if (err < 0)
        return err;

    // Set basic card identification strings
    strcpy(my_card->driver, DRIVER_NAME);
    strcpy(my_card->shortname, CARD_NAME);
    strcpy(my_card->longname, "My Custom ALSA Audio Card");

    // Create PCM device
    err = snd_pcm_new(my_card, "My PCM", 0, 1, 0, &my_pcm);
    if (err < 0)
        goto fail;

    // Set PCM ops for playback stream
    snd_pcm_set_ops(my_pcm, SNDRV_PCM_STREAM_PLAYBACK, &my_pcm_ops);

    my_pcm->private_data = NULL;
    my_pcm->info_flags = 0;
    strcpy(my_pcm->name, "My PCM Playback");

    // Allocate continuous memory (no longer using deprecated API)
    snd_pcm_lib_preallocate_pages_for_all(my_pcm,
                                          SNDRV_DMA_TYPE_CONTINUOUS,
                                          NULL,
                                          64 * 1024, 64 * 1024);

    // Register sound card
    err = snd_card_register(my_card);
    if (err < 0)
        goto fail;

    pr_info("MyAudioCard: ALSA dummy audio driver loaded successfully\n");
    return 0;

fail:
    snd_card_free(my_card);
    return err;
}

static void __exit my_audio_exit(void)
{
    snd_card_free(my_card);
    pr_info("MyAudioCard: ALSA dummy audio driver unloaded\n");
}

module_init(my_audio_init);
module_exit(my_audio_exit);

MODULE_AUTHOR("TechDhaba");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Minimal ALSA Dummy Audio Driver for x86 Linux");
