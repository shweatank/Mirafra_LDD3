#include <stdio.h>
#include <stdlib.h>
#include <mpg123.h>
#include <alsa/asoundlib.h>

#define PCM_DEVICE "default"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.mp3>\n", argv[0]);
        return 1;
    }

    // MPG123 variables
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err;
    int channels, encoding;
    long rate;

    // ALSA variables
    snd_pcm_t *pcm;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;

    // Initialize mpg123
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    mpg123_open(mh, argv[1]);
    mpg123_getformat(mh, &rate, &channels, &encoding);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char *) malloc(buffer_size);

    // Initialize ALSA
    snd_pcm_open(&pcm, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm, params);
    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, channels);
    snd_pcm_hw_params_set_rate_near(pcm, params, (unsigned int *) &rate, 0);
    snd_pcm_hw_params(pcm, params);

    // Playback loop
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
        snd_pcm_writei(pcm, buffer, done / 2); // 2 bytes per sample (S16_LE)
    }

    // Clean up
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    free(buffer);

    return 0;
}

