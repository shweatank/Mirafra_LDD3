#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define AUDIO_DEVICE "plughw:0,0"  // Use correct device from arecord -l
#define SAMPLE_RATE 44100
#define CHANNELS 1
#define FORMAT SND_PCM_FORMAT_S16_LE
#define SECONDS 5

int main() {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    int err;

    int frames_per_buffer = 1024;
    int16_t *buffer = malloc(frames_per_buffer * sizeof(int16_t));

    if ((err = snd_pcm_open(&handle, AUDIO_DEVICE, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "Cannot open device: %s\n", snd_strerror(err));
        return 1;
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, FORMAT);
    snd_pcm_hw_params_set_rate(handle, params, SAMPLE_RATE, 0);
    snd_pcm_hw_params_set_channels(handle, params, CHANNELS);
    snd_pcm_hw_params(handle, params);
    snd_pcm_hw_params_free(params);
    snd_pcm_prepare(handle);

    FILE *file = fopen("mic_output.raw", "wb");
    int total_frames = SAMPLE_RATE * SECONDS;
    int frames_read = 0;

    while (frames_read < total_frames) {
        int n = snd_pcm_readi(handle, buffer, frames_per_buffer);
        if (n > 0) {
            fwrite(buffer, sizeof(int16_t), n, file);
            frames_read += n;
        }
    }

    fclose(file);
    snd_pcm_close(handle);
    free(buffer);
    printf("Audio captured to mic_output.raw\n");
    return 0;
}

