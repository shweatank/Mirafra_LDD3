#include <stdio.h>
#include <stdlib.h>
#include <mpg123.h>
#include <alsa/asoundlib.h>

#define PCM_DEVICE "default"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file.mp3> <speed (e.g., 1.0, 0.5, 2.0)>\n", argv[0]);
        return 1;
    }

    const char *mp3_file = argv[1];
    float speed = atof(argv[2]);  // Convert speed string to float

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

    // --- Initialize mpg123 ---
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    mpg123_open(mh, mp3_file);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    // Change playback speed by modifying rate
    long adjusted_rate = (long)(rate * speed);

    buffer_size = mpg123_outblock(mh);
    buffer = malloc(buffer_size);

    // --- Initialize ALSA ---
    snd_pcm_open(&pcm, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm, params);
    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, channels);
    snd_pcm_hw_params_set_rate_near(pcm, params, (unsigned int *) &adjusted_rate, 0);
    snd_pcm_hw_params(pcm, params);

    // --- Playback loop ---
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
        snd_pcm_writei(pcm, buffer, done / (channels * 2));  // 2 bytes per sample
    }

    // --- Clean up ---
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    free(buffer);

    return 0;
}

