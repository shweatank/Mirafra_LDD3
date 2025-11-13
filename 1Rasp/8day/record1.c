#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define RATE 44100
#define CHANNELS 2
#define FORMAT SND_PCM_FORMAT_S16_LE
#define SECONDS 5
#define BUFFER_FRAMES 600

int main() {
    snd_pcm_t *capture_handle, *playback_handle;
    snd_pcm_hw_params_t *hw_params;
    char *buffer;
    int err, buffer_size;

    // Open capture device
    snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, FORMAT);
    snd_pcm_hw_params_set_rate(capture_handle, hw_params, RATE, 0);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, CHANNELS);
    snd_pcm_hw_params(capture_handle, hw_params);
    snd_pcm_hw_params_free(hw_params);
    snd_pcm_prepare(capture_handle);

    // Open playback device
    snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(playback_handle, hw_params);
    snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playback_handle, hw_params, FORMAT);
    snd_pcm_hw_params_set_rate(playback_handle, hw_params, RATE, 0);
    snd_pcm_hw_params_set_channels(playback_handle, hw_params, CHANNELS);
    snd_pcm_hw_params(playback_handle, hw_params);
    snd_pcm_hw_params_free(hw_params);
    snd_pcm_prepare(playback_handle);

    buffer_size = BUFFER_FRAMES * snd_pcm_format_width(FORMAT) / 8 * CHANNELS;
    buffer = (char *)malloc(buffer_size);

    int total_frames = RATE * SECONDS;
    for (int i = 0; i < total_frames / BUFFER_FRAMES; ++i) {
        if ((err = snd_pcm_readi(capture_handle, buffer, BUFFER_FRAMES)) != BUFFER_FRAMES) {
            fprintf(stderr, "read from audio interface failed (%s)\n", snd_strerror(err));
        }
	usleep(3000); 
        err = snd_pcm_writei(playback_handle, buffer, BUFFER_FRAMES);
	if (err == -EPIPE) 
	{
	        fprintf(stderr, "XRUN (underrun)\n");
    		snd_pcm_prepare(playback_handle);
	} 
	else if (err < 0) 
	{
   	 fprintf(stderr, "Error writing to audio interface: %s\n", snd_strerror(err));
	}

    }

    free(buffer);
    snd_pcm_close(capture_handle);
    snd_pcm_close(playback_handle);

    return 0;
}
