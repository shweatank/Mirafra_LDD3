// record_audio.c - Simple ALSA-based audio recorder that saves to a WAV file

#include <stdio.h>              // Standard I/O (fopen, fwrite, etc.)
#include <stdlib.h>             // For malloc, free, exit
#include <alsa/asoundlib.h>     // ALSA audio library (snd_pcm_* APIs)

// ---------- Configuration Macros ----------
#define AUDIO_FILE "recorded.wav"   // Output file name
#define SAMPLE_RATE 44100           // Sampling rate (44.1 kHz - CD quality)
#define CHANNELS 2                  // Number of channels (2 = stereo)
#define SECONDS 10                  // Recording duration in seconds

int main() {
    snd_pcm_t *pcm_handle;           // Handle to ALSA PCM device
    snd_pcm_hw_params_t *params;     // Hardware parameter object
    unsigned int rate = SAMPLE_RATE; // Actual sample rate to configure
    snd_pcm_uframes_t frames = 32;   // Number of frames per period (buffer granularity)
    int rc;                          // Return code for ALSA function calls

    // Open the output file for writing binary data
    FILE *fp = fopen(AUDIO_FILE, "wb");
    if (!fp) {                       // If file couldn't be opened
        perror("fopen");             // Print error message
        return 1;                    // Exit with error code
    }

    // Open the default ALSA device in capture mode (recording)
    rc = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_CAPTURE, 0);

    // Allocate memory for ALSA hardware parameters structure
    snd_pcm_hw_params_alloca(&params);

    // Initialize parameters object with default values
    snd_pcm_hw_params_any(pcm_handle, params);

    // Set access type: interleaved (data for channels stored alternately)
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    // Set audio format: signed 16-bit little-endian (CD quality format)
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);

    // Set number of channels (1 = mono, 2 = stereo)
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);

    // Set sampling rate as close as possible to SAMPLE_RATE
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, NULL);

    // Set period size (number of frames per buffer read/write)
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, NULL);

    // Apply the hardware parameters to the PCM device
    snd_pcm_hw_params(pcm_handle, params);

    // Calculate buffer size in bytes:
    // frames * channels * bytes_per_sample (16 bits = 2 bytes)
    int size = frames * CHANNELS * 2;

    // Allocate buffer to store one period of audio
    char *buffer = (char *)malloc(size);

    // Calculate number of loops needed to capture SECONDS worth of audio
    // (Total samples = SAMPLE_RATE * SECONDS)
    // Divide by frames per loop
    int loops = (SAMPLE_RATE * SECONDS) / frames;

    // Skip 44 bytes at start of file for WAV header (we'll write it later)
    fseek(fp, 44, SEEK_SET);

    // ----------- Main Recording Loop -----------
    while (loops-- > 0) {
        // Capture 'frames' samples from the PCM device into buffer
        rc = snd_pcm_readi(pcm_handle, buffer, frames);

        // Write the captured samples to output file
        fwrite(buffer, 1, size, fp);
    }

    // Free the temporary buffer memory
    free(buffer);

    // Stop capturing immediately and discard any unread frames
    snd_pcm_drop(pcm_handle);

    // Close the PCM capture device
    snd_pcm_close(pcm_handle);

    // ----------- Write WAV Header -----------
    // After recording, we go back to the beginning of the file and write header
    fseek(fp, 0, SEEK_SET);

    // Calculate sizes for WAV header fields
    int data_chunk_size = SAMPLE_RATE * SECONDS * CHANNELS * 2; // total audio data size in bytes
    int file_size = data_chunk_size + 44 - 8; // total file size - 8 (WAV spec requirement)

    // Write "RIFF" chunk descriptor
    fwrite("RIFF", 1, 4, fp);          // Chunk ID
    fwrite(&file_size, 4, 1, fp);      // Chunk size (file size - 8)
    fwrite("WAVEfmt ", 1, 8, fp);      // Format and Subchunk1 ID

    // Write "fmt " subchunk
    int fmt_chunk_size = 16;           // Subchunk1 size (PCM = 16)
    short audio_format = 1;            // Audio format (1 = PCM/uncompressed)
    short num_channels = CHANNELS;     // Number of channels (mono=1, stereo=2)
    int byte_rate = SAMPLE_RATE * CHANNELS * 2;  // Bytes per second
    short block_align = CHANNELS * 2;            // Block alignment (bytes per sample frame)
    short bits_per_sample = 16;                  // Bits per sample (16 bits)

    fwrite(&fmt_chunk_size, 4, 1, fp);  // Subchunk1 size
    fwrite(&audio_format, 2, 1, fp);    // Audio format
    fwrite(&num_channels, 2, 1, fp);    // Number of channels
    fwrite(&rate, 4, 1, fp);            // Sample rate
    fwrite(&byte_rate, 4, 1, fp);       // Byte rate
    fwrite(&block_align, 2, 1, fp);     // Block align
    fwrite(&bits_per_sample, 2, 1, fp); // Bits per sample

    // Write "data" subchunk
    fwrite("data", 1, 4, fp);           // Subchunk2 ID
    fwrite(&data_chunk_size, 4, 1, fp); // Subchunk2 size (number of data bytes)

    // Close the output file (recorded.wav now contains valid WAV file)
    fclose(fp);

    // End of program
    return 0;
}

