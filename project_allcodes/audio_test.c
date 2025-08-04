#include <stdio.h>                      // Standard I/O for file operations, printf
#include <stdlib.h>                    // Standard library (malloc, free, exit)
#include <alsa/asoundlib.h>           // ALSA library header
#include <string.h>                   // For memory and string functions

// Define audio parameters
int SAMPLE_RATE = 44100;              // Sampling rate: 44100 samples per second
int CHANNELS = 1;                     // Mono channel (1 = mono, 2 = stereo)
int BITS_PER_SAMPLE = 16;             // Each sample is 16 bits (2 bytes)

#define RECORD_SECONDS 5              // Number of seconds to record
#define FILENAME "recorded.wav"       // Output filename

// Function declarations
int record_audio(const char *filename);
int play_audio(const char *filename); // Declared but not used in this code

int main() {
    // Inform user that recording will start
    printf("Recording and playing back %d seconds of audio...\n", RECORD_SECONDS);

    // Call record_audio function and handle failure
    if (record_audio(FILENAME) != 0) {
        fprintf(stderr, "Recording failed.\n"); // Error message
        return 1;                               // Exit with error code
    }

    printf("Recording Is Done.\n");             // Confirmation
    return 0;                                   // Exit successfully
}

int record_audio(const char *filename)
{
    snd_pcm_t *pcm_handle;                      // Handle for PCM device
    snd_pcm_hw_params_t *params;                // ALSA hardware parameter structure
    unsigned int sample_rate = SAMPLE_RATE;     // Local variable for sample rate
    int rc, dir;                                // rc: return code, dir: direction variable
    snd_pcm_uframes_t frames = 32;              // Number of frames per period
    int size;                                   // Size of the buffer in bytes
    char *buffer;                               // Audio buffer pointer
    int total_frames = SAMPLE_RATE * RECORD_SECONDS; // Total frames to record
    int total_data_bytes = total_frames * CHANNELS * BITS_PER_SAMPLE / 8; // Total size in bytes

    // Open output file for writing (binary mode)
    FILE *fp = fopen(filename, "wb+");
    if (!fp) {
        perror("File open failed");             // Error if file cannot be opened
        return -1;
    }

    // Open PCM device for audio capture ("plughw:0,7" = card 0, device 7)
    rc = snd_pcm_open(&pcm_handle, "plughw:0,7", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open PCM device: %s\n", snd_strerror(rc)); // ALSA error message
        fclose(fp);                      // Close file if PCM open fails
        return -1;
    }

    // Allocate memory for hardware parameters object on the stack
    snd_pcm_hw_params_alloca(&params);

    // Fill it in with default values
    snd_pcm_hw_params_any(pcm_handle, params);

    // Set access type: interleaved = all channels in single buffer
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    // Set audio format: 16-bit little endian
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);

    // Set number of channels (mono or stereo)
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);

    // Set sample rate near desired rate (may adjust slightly)
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate, &dir);

    // Set the period size (number of frames per buffer)
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, &dir);

    // Apply the hardware parameters to the PCM device
    snd_pcm_hw_params(pcm_handle, params);

    // Get the final buffer size in frames
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);

    // Calculate buffer size in bytes: frames × channels × bytes/sample
    size = frames * CHANNELS * BITS_PER_SAMPLE / 8;

    // Allocate buffer dynamically
    buffer = (char *)malloc(size);
    if (!buffer) {
        perror("Buffer malloc failed");         // Handle memory allocation failure
        snd_pcm_close(pcm_handle);              // Cleanup
        fclose(fp);
        return -1;
    }

    // Leave space at beginning of file for WAV header (to be filled later)
    fseek(fp, 44, SEEK_SET);

    int frames_recorded = 0;                    // Track number of recorded frames

    // Begin recording loop until total_frames is reached
    while (frames_recorded < total_frames) {
        // Read frames from the audio interface
        rc = snd_pcm_readi(pcm_handle, buffer, frames);

        if (rc == -EPIPE) {
            // Buffer overrun/underrun occurred
            snd_pcm_prepare(pcm_handle);        // Recover from error
            continue;
        } else if (rc < 0) {
            // Generic ALSA error
            fprintf(stderr, "Read error: %s\n", snd_strerror(rc));
        } else if (rc > 0) {
            // Successfully read audio; write to file
            fwrite(buffer, CHANNELS * BITS_PER_SAMPLE / 8, rc, fp);
            frames_recorded += rc;              // Update recorded frame count
        }
    }

    // Stop the PCM capture stream
    snd_pcm_drop(pcm_handle);

    // Close PCM device
    snd_pcm_close(pcm_handle);

    // Free dynamically allocated audio buffer
    free(buffer);

    // Go back to beginning of file to write actual WAV header
    fseek(fp, 0, SEEK_SET);

    // Compute WAV header fields
    int byte_rate = SAMPLE_RATE * CHANNELS * BITS_PER_SAMPLE / 8;
    short block_align = CHANNELS * BITS_PER_SAMPLE / 8;
    int data_chunk_size = frames_recorded * block_align;
    int chunk_size = 36 + data_chunk_size;

    // Write standard 44-byte WAV header
    fwrite("RIFF", 1, 4, fp);                   // ChunkID
    fwrite(&chunk_size, 4, 1, fp);              // ChunkSize
    fwrite("WAVE", 1, 4, fp);                   // Format
    fwrite("fmt ", 1, 4, fp);                   // Subchunk1ID
    int subchunk1_size = 16;
    short audio_format = 1;                     // PCM = 1 (linear quantization)
    fwrite(&subchunk1_size, 4, 1, fp);          // Subchunk1Size
    fwrite(&audio_format, 2, 1, fp);            // AudioFormat
    fwrite(&CHANNELS, 2, 1, fp);                // NumChannels
    fwrite(&SAMPLE_RATE, 4, 1, fp);             // SampleRate
    fwrite(&byte_rate, 4, 1, fp);               // ByteRate
    fwrite(&block_align, 2, 1, fp);             // BlockAlign
    fwrite(&BITS_PER_SAMPLE, 2, 1, fp);         // BitsPerSample
    fwrite("data", 1, 4, fp);                   // Subchunk2ID
    fwrite(&data_chunk_size, 4, 1, fp);         // Subchunk2Size

    // Close file after header and data written
    fclose(fp);

    return 0;                                   // Success
}

