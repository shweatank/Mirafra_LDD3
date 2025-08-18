#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PCM_DEVICE "plughw:0,0"
#define SAMPLE_RATE 44100
#define CHANNELS 1
#define SECONDS 5
#define FORMAT SND_PCM_FORMAT_S16_LE

int main() {
    snd_pcm_t *capture_handle, *playback_handle;
    snd_pcm_hw_params_t *params;
    int pcm, frames;
    char *buffer;
    int buffer_size;

    unsigned int rate = SAMPLE_RATE;
    snd_pcm_uframes_t period_size = 1024;

    buffer_size = period_size * CHANNELS * 2;  // 2 bytes/sample (S16_LE)
    buffer = (char *) malloc(buffer_size);

    int total_frames = SAMPLE_RATE * SECONDS;
    int frames_recorded = 0;

    // ======== CAPTURE SETUP ========
    if ((pcm = snd_pcm_open(&capture_handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "ERROR: Can't open capture device: %s\n", snd_strerror(pcm));
        return 1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(capture_handle, params);
    snd_pcm_hw_params_set_access(capture_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(capture_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(capture_handle, params, &rate, NULL);
    snd_pcm_hw_params_set_period_size_near(capture_handle, params, &period_size, NULL);
    snd_pcm_hw_params(capture_handle, params);

    printf("Recording %d seconds of audio...\n", SECONDS);
    FILE *fp = fopen("record.raw", "wb");

    while (frames_recorded < total_frames) {
        frames = snd_pcm_readi(capture_handle, buffer, period_size);
        if (frames > 0) {
            fwrite(buffer, CHANNELS * 2, frames, fp);
            frames_recorded += frames;
        }
    }

    fclose(fp);
    snd_pcm_close(capture_handle);
    printf("Recording complete.\n");

    // ======== PLAYBACK SETUP ========
    if ((pcm = snd_pcm_open(&playback_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "ERROR: Can't open playback device: %s\n", snd_strerror(pcm));
        return 1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(playback_handle, params);
    snd_pcm_hw_params_set_access(playback_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playback_handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(playback_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(playback_handle, params, &rate, NULL);
    snd_pcm_hw_params_set_period_size_near(playback_handle, params, &period_size, NULL);
    snd_pcm_hw_params(playback_handle, params);

    fp = fopen("record.raw", "rb");
    printf("Playing back recorded audio...\n");
    while ((frames = fread(buffer, CHANNELS * 2, period_size, fp)) > 0) {
        pcm = snd_pcm_writei(playback_handle, buffer, frames);
        if (pcm < 0) snd_pcm_prepare(playback_handle);
    }

    fclose(fp);
    snd_pcm_drain(playback_handle);
    snd_pcm_close(playback_handle);
    free(buffer);
    printf("Playback complete.\n");

    return 0;
}

