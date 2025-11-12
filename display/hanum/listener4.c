#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#define SAMPLE_RATE 44100
#define CHANNELS 2
#define FRAME_SIZE (2*CHANNELS) // 16-bit PCM
int main(int argc, char *argv[]) {
snd_pcm_t *handle;
snd_pcm_hw_params_t *params;
snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
snd_pcm_hw_params_malloc(&params);
snd_pcm_hw_params_any(handle, params);
snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
snd_pcm_hw_params_set_channels(handle, params, CHANNELS);
snd_pcm_hw_params_set_rate(handle, params, SAMPLE_RATE, 0);
snd_pcm_hw_params(handle, params);
snd_pcm_hw_params_free(params);
snd_pcm_prepare(handle);
char *cmd="ffmpeg -nostdin "
"-i tcp://192.168.0.165:5000 "
"-f s16le "
"-acodec pcm_s16le "
"-ac 2 "
"-ar 44100 "
"-";
FILE *pipe = popen(cmd, "r");
if (!pipe) return 1;
int16_t buffer[1024 * CHANNELS];
while (fread(buffer, sizeof(int16_t), 1024 * CHANNELS, pipe) > 0) {
snd_pcm_writei(handle, buffer, 1024);
}
pclose(pipe);
snd_pcm_drain(handle);
snd_pcm_close(handle);
return 0;
}
