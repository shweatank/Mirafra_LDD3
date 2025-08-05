#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <stdint.h>

#define FILENAME "audio.wav"
#define SAMPLE_RATE 44100
#define CHANNELS 1
#define BITS_PER_SAMPLE 16
#define RECORD_SECONDS 5

int write_wav_header(FILE *file, int sample_rate, int bits_per_sample, int channels, int data_size) {
    int byte_rate = sample_rate * channels * bits_per_sample / 8;
    int block_align = channels * bits_per_sample / 8;
    int chunk_size = 36 + data_size;

    fwrite("RIFF", 1, 4, file);
    fwrite(&chunk_size, 4, 1, file);
    fwrite("WAVE", 1, 4, file);

    // fmt subchunk
    fwrite("fmt ", 1, 4, file);
    int subchunk1_size = 16;
    short audio_format = 1; // PCM
    fwrite(&subchunk1_size, 4, 1, file);
    fwrite(&audio_format, 2, 1, file);
    fwrite(&channels, 2, 1, file);
    fwrite(&sample_rate, 4, 1, file);
    fwrite(&byte_rate, 4, 1, file);
    fwrite(&block_align, 2, 1, file);
    fwrite(&bits_per_sample, 2, 1, file);

    // data subchunk
    fwrite("data", 1, 4, file);
    fwrite(&data_size, 4, 1, file);

    return 0;
}

int record_audio(const char *filename) {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate = SAMPLE_RATE;
    int dir;
    int rc;

    snd_pcm_uframes_t frames = 32;
    int frame_size = CHANNELS * BITS_PER_SAMPLE / 8;
    int buffer_size = RECORD_SECONDS * SAMPLE_RATE * frame_size;
    char *buffer = (char *)malloc(buffer_size);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open PCM device: %s\n", snd_strerror(rc));
        free(buffer);
        return -1;
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr, "Unable to set HW parameters: %s\n", snd_strerror(rc));
        free(buffer);
        return -1;
    }

    int total_frames = SAMPLE_RATE * RECORD_SECONDS;
    int frames_read = 0;

    printf("Recording for %d seconds...\n", RECORD_SECONDS);

    while (frames_read < total_frames) {
        int frames_to_read = (total_frames - frames_read) > frames ? frames : (total_frames - frames_read);
        rc = snd_pcm_readi(handle, buffer + frames_read * frame_size, frames_to_read);
        if (rc == -EPIPE) {
            fprintf(stderr, "Overrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "Error reading from PCM device: %s\n", snd_strerror(rc));
        } else {
            frames_read += rc;
        }
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    snd_pcm_hw_params_free(params);

    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Unable to open file for writing\n");
        free(buffer);
        return -1;
    }

    write_wav_header(file, SAMPLE_RATE, BITS_PER_SAMPLE, CHANNELS, buffer_size);
    fwrite(buffer, 1, buffer_size, file);
    fclose(file);
    free(buffer);

    printf("Recording saved to '%s'\n", filename);
    return 0;
}

int main() {
    return record_audio(FILENAME);
}

