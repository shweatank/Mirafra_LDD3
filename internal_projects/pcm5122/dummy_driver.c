#include<linux/module.h>
#include<linux/init.h>
#include<linux/sound.h>
#include<sound/core.h>
#include<sound/pcm.h>
#include<sound/initval.h>

static struct snd_pcm_hardware my_pcm_hardware={
      .info =SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_MMAP,
      .formats = SNDRV_PCM_FMTBIT_S16_LE, 
      .rates = SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000, 
      .rate_min = 44100, 
      .rate_max = 48000, 
      .channels_min = 2, 
      .channels_max = 2, 
      .buffer_bytes_max = 64 * 1024, 
      .period_bytes_min = 1024, 
      .period_bytes_max = 8192, 
      .periods_min = 2, 
      .periods_max = 8, 
};
static struct snd_card *card;

static int my_pcm_open(struct snd_pcm_substream *sb)
{
	struct snd_pcm_runtime *runtime=sb->runtime;
	runtime->hw =my_pcm_hardware;
	return 0;
}
static int my_pcm_close(struct snd_pcm_substream *substream) {
     return 0;
}
static int my_hw_params(struct snd_pcm_substream *substream,
                        struct snd_pcm_hw_params *params)
{
    size_t size = params_buffer_bytes(params);
    return snd_pcm_lib_malloc_pages(substream, size);
}

static struct snd_pcm_ops my_pcm_ops = {
     .open = my_pcm_open,
     .close = my_pcm_close,
     .ioctl = snd_pcm_lib_ioctl,
     .hw_params = my_hw_params,
//     .hw_free = snd_pcm_lib_free_pages,
     .prepare = NULL,                       // We can implement audio preparation later
     .trigger = NULL,                       // Handle playback start/stop
     .pointer = NULL,                       // Return playback position
};

 static int __init my_alsa_init(void)
{
//	struct snd_card *card;
	struct snd_pcm *pcm;
        int err;
	err=snd_card_new(NULL,-1,"MYALSA",THIS_MODULE,0,&card);
       if(err<0)
	       return err;
	err=snd_pcm_new(card,"MY PCM",0,1,1,&pcm);
	if(err<0)
		goto error;
	pcm->private_data=NULL;
	pcm->info_flags=0;
	strcpy(pcm->name,"MY PCM Device");

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK,&my_pcm_ops);

	strcpy(card->driver, "MYALSA");
        strcpy(card->shortname, "My ALSA Virtual Card");
         strcpy(card->longname, "My Simple ALSA PCM Card");

 
	err=snd_card_register(card);
        if(err<0)
		goto error;
	pr_info("MY ALSA dri ver loader\n");

	return 0;
error:
	snd_card_free(card);
	return err;
}

static void __exit my_alsa_exit(void)
{
	snd_card_free(card);
	pr_info("MY ALSA driver unloaded\n");
}

module_init(my_alsa_init);
module_exit(my_alsa_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MK");
MODULE_DESCRIPTION("Simple ALSA Sound Driver");

