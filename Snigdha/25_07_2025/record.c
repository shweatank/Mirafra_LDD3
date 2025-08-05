#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>

#define SAMPLE_RATE 44100
#define CHANNELS 1
#define SECONDS 5
#define FORMAT SND_PCM_FORMAT_S16_LE
#define FILENAME "recorded.wav"

void write_wav_header(FILE *file, int sample_rate, int channels, int bits_per_sample, int data_size) {
    int byte_rate = sample_rate * channels * bits_per_sample / 8;
    int block_align = channels * bits_per_sample / 8;
    int chunk_size = 36 + data_size;

    // RIFF header
    fwrite("RIFF", 1, 4, file);
    fwrite(&chunk_size, 4, 1, file);
    fwrite("WAVE", 1, 4, file);

    // fmt subchunk
    fwrite("fmt ", 1, 4, file);
    int subchunk1_size = 16;
    short audio_format = 1;
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
}

int main() {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = SAMPLE_RATE;
    int dir;
    int rc;

    int frames = 32;
    int frame_size = CHANNELS * 2;  // 2 bytes per sample (16 bits)
    char *buffer = (char *) malloc(frames * frame_size);

    // Open PCM device for recording (capture)
    rc = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open PCM device: %s\n", snd_strerror(rc));
        return 1;
    }

    // Allocate hardware parameters
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, &dir);
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, (snd_pcm_uframes_t*)&frames, &dir);
    snd_pcm_hw_params(pcm_handle, params);

    // Open WAV file
    FILE *wav = fopen(FILENAME, "wb");
    if (!wav) {
        perror("fopen");
        return 1;
    }

    // Placeholder WAV header (we will overwrite later)
    int header_size = 44;
    fseek(wav, header_size, SEEK_SET);

    int total_bytes = 0;
    int loops = SAMPLE_RATE * SECONDS / frames;

    // Start capturing
    for (int i = 0; i < loops; ++i) {
        rc = snd_pcm_readi(pcm_handle, buffer, frames);
        if (rc == -EPIPE) {
            snd_pcm_prepare(pcm_handle);
        } else if (rc < 0) {
            fprintf(stderr, "Error reading from PCM device: %s\n", snd_strerror(rc));
        } else {
            fwrite(buffer, frame_size, rc, wav);
            total_bytes += rc * frame_size;
        }
    }

    // Update WAV header
    fseek(wav, 0, SEEK_SET);
    write_wav_header(wav, SAMPLE_RATE, CHANNELS, 16, total_bytes);

    // Cleanup
    fclose(wav);
    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    snd_pcm_hw_params_free(params);
    free(buffer);

    printf("Recording saved to %s\n", FILENAME);
    return 0;
}
