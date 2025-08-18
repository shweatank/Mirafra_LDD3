#include <alsa/asoundlib.h>  // ALSA API header for audio device interaction
#include <stdio.h>             // Standard I/O functions
#include <stdlib.h>            // Memory allocation and general utilities
#include <string.h>            // String operations

// Define ALSA device to use
#define PCM_DEVICE "plughw:0,0"  // ALSA PCM device for hardware 0, subdevice 0
#define SAMPLE_RATE 88200             //44100        // Sample rate: 44.1 kHz
#define CHANNELS 1               // Mono audio
#define SECONDS 5                // Record/playback duration in seconds
#define FORMAT SND_PCM_FORMAT_S16_LE  // Signed 16-bit Little Endian format

int main() {
    snd_pcm_t *capture_handle, *playback_handle; // ALSA PCM handles for capture and playback
    snd_pcm_hw_params_t *params;                 // Hardware parameters structure
    int pcm, frames;                             // Variables for return values and frame count
    char *buffer;                                // Audio data buffer
    int buffer_size;

    unsigned int rate = SAMPLE_RATE;             // Copy sample rate to modifiable variable
    snd_pcm_uframes_t period_size = 1024;        // ALSA frame size per read/write operation

    // Calculate buffer size in bytes: frames * channels * bytes_per_sample
    buffer_size = period_size * CHANNELS * 2;    // 2 bytes for S16_LE
    buffer = (char *) malloc(buffer_size);       // Allocate memory for audio buffer

    int total_frames = SAMPLE_RATE * SECONDS;    // Total number of frames to record
    int frames_recorded = 0;                     // Frame counter

    // ======== CAPTURE SETUP ========
    // Open PCM device for audio capture
    if ((pcm = snd_pcm_open(&capture_handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "ERROR: Can't open capture device: %s\n", snd_strerror(pcm));
        return 1;
    }

    // Allocate hardware parameter object
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(capture_handle, params);  // Initialize params with default values

    // Set hardware parameters for interleaved access
    snd_pcm_hw_params_set_access(capture_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    // Set format: 16-bit signed little endian
    snd_pcm_hw_params_set_format(capture_handle, params, FORMAT);
    // Set number of audio channels
    snd_pcm_hw_params_set_channels(capture_handle, params, CHANNELS);
    // Set sample rate (may be adjusted by driver)
    snd_pcm_hw_params_set_rate_near(capture_handle, params, &rate, NULL);
    // Set period size in frames
    snd_pcm_hw_params_set_period_size_near(capture_handle, params, &period_size, NULL);
    // Apply hardware parameters to capture device
    snd_pcm_hw_params(capture_handle, params);

    printf("Recording %d seconds of audio...\n", SECONDS);
    FILE *fp = fopen("record.raw", "wb");  // Open file to save raw PCM audio

    // Record audio until required frame count is reached
    while (frames_recorded < total_frames) {
        frames = snd_pcm_readi(capture_handle, buffer, period_size);  // Read audio frames
        if (frames > 0) {
            fwrite(buffer, CHANNELS * 2, frames, fp);  // Write raw PCM to file
            frames_recorded += frames;                // Update frame counter
        }
    }

    fclose(fp);                    // Close the file
    snd_pcm_close(capture_handle);  // Close capture device
    printf("Recording complete.\n");

    // ======== PLAYBACK SETUP ========
    // Open PCM device for playback
    if ((pcm = snd_pcm_open(&playback_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "ERROR: Can't open playback device: %s\n", snd_strerror(pcm));
        return 1;
    }

    // Reuse params structure for playback
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(playback_handle, params);
    snd_pcm_hw_params_set_access(playback_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playback_handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(playback_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(playback_handle, params, &rate, NULL);
    snd_pcm_hw_params_set_period_size_near(playback_handle, params, &period_size, NULL);
    snd_pcm_hw_params(playback_handle, params);

    fp = fopen("record.raw", "rb");  // Open recorded file for playback
    printf("Playing back recorded audio...\n");

    // Read file data and send to ALSA playback device
    while ((frames = fread(buffer, CHANNELS * 2, period_size, fp)) > 0) {
        pcm = snd_pcm_writei(playback_handle, buffer, frames);  // Write frames to audio device
        if (pcm < 0) snd_pcm_prepare(playback_handle);          // Recover from buffer underrun
    }

    fclose(fp);                          // Close playback file
    snd_pcm_drain(playback_handle);      // Wait for buffer to finish playing
    snd_pcm_close(playback_handle);      // Close playback device
    free(buffer);                        // Free allocated buffer
    printf("Playback complete.\n");

    return 0;
}

