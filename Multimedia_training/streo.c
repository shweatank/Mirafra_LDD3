#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define BUFFER_SIZE 4096

void play_audio(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return;
    }

    // Skip WAV header (assumes 44 bytes)
    fseek(fp, 44, SEEK_SET);

    snd_pcm_t *pcm;
    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Error opening PCM device\n");
        fclose(fp);
        return;
    }

    // Set PCM parameters: 16-bit LE, stereo, 44100 Hz
    if (snd_pcm_set_params(pcm,
                           SND_PCM_FORMAT_S16_LE,
                           SND_PCM_ACCESS_RW_INTERLEAVED,
                           2,           // stereo
                           48000,       // sample rate
                           1,           // allow resampling
                           500000) < 0) // 0.5 sec latency
    {
        fprintf(stderr, "Error setting PCM parameters\n");
        snd_pcm_close(pcm);
        fclose(fp);
        return;
    }

    short buffer[BUFFER_SIZE];
    int frames;

    while (!feof(fp)) {
        size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
        if (bytes_read == 0) break;

        frames = snd_pcm_writei(pcm, buffer, bytes_read / 4); // 2 bytes/sample * 2 channels
        if (frames < 0) {
            frames = snd_pcm_recover(pcm, frames, 0);
            if (frames < 0) {
                fprintf(stderr, "Error during playback\n");
                break;
            }
        }
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(fp);

    printf("Playback complete.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <wav_file>\n", argv[0]);
        return 1;
    }

    play_audio(argv[1]);
    return 0;
}

