#include <alsa/asoundlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <wav file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    // Skip WAV header (44 bytes)
    fseek(fp, 44, SEEK_SET);

    snd_pcm_t *pcm;
    int err = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_open error: %s\n", snd_strerror(err));
        return 1;
    }

    // Setup: S16_LE = 16-bit signed little endian
    err = snd_pcm_set_params(pcm,
                             SND_PCM_FORMAT_S16_LE,
                             SND_PCM_ACCESS_RW_INTERLEAVED,
                             2,              // 2 channels (stereo)
                             44100,          // sample rate
                             1,              // soft resample
                             500000);        // latency (0.5 sec)

    if (err < 0) {
        fprintf(stderr, "snd_pcm_set_params error: %s\n", snd_strerror(err));
        return 1;
    }

    short buffer[4096];  // buffer for 16-bit audio
    while (!feof(fp)) {
        size_t samples = fread(buffer, sizeof(short), 4096, fp);
        if (samples == 0) break;

        int frames = snd_pcm_writei(pcm, buffer, samples / 2); // 2 samples per frame (stereo)
        if (frames < 0)
            frames = snd_pcm_recover(pcm, frames, 0);
        if (frames < 0) {
            fprintf(stderr, "snd_pcm_writei failed: %s\n", snd_strerror(frames));
            break;
        }
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(fp);

    return 0;
}

