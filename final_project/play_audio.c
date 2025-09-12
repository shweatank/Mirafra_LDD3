// play_audio.c
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define AUDIO_FILE "decoded.wav"

int main() {
    snd_pcm_t *pcm;
    FILE *fp = fopen(AUDIO_FILE, "rb");
    if (!fp) { perror("fopen"); return 1; }

    fseek(fp, 44, SEEK_SET); // Skip WAV header

    snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(pcm,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        2, 44100, 1, 500000);

    char buffer[4096];
    int frames;
    while ((frames = fread(buffer, 1, sizeof(buffer), fp)) > 0)
        snd_pcm_writei(pcm, buffer, frames / 4); // 2 channels * 2 bytes

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(fp);
    return 0;
}

