#include <alsa/asoundlib.h>

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
    snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(pcm,
        SND_PCM_FORMAT_U8,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1,              // mono
        44100,          // sample rate
        1,              // soft resample
        500000);        // latency in us

    unsigned char buffer[4096];
    int frames;

    while (!feof(fp)) {
        size_t len = fread(buffer, 1, sizeof(buffer), fp);
        frames = snd_pcm_writei(pcm, buffer, len);
        if (frames < 0) frames = snd_pcm_recover(pcm, frames, 0);
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(fp);

    return 0;
}
