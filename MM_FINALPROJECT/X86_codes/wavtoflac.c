// wav_to_flac.c - Convert a WAV file to FLAC using libFLAC

#include <FLAC/stream_encoder.h> // FLAC encoder API
#include <stdio.h>               // Standard I/O (fopen, fread, printf, perror)
#include <stdlib.h>              // malloc, free, exit
#include <stdint.h>              // int16_t, int32_t

// ---------- File Paths ----------
#define INPUT_WAV "recorded.wav"  // Input WAV file
#define OUTPUT_FLAC "output.flac" // Output FLAC file

int main() {
    // Open WAV file for reading
    FILE *wav = fopen(INPUT_WAV, "rb");
    if (!wav) { 
        perror("WAV open"); 
        return 1; 
    }

    // Skip the first 44 bytes of WAV header (PCM data starts after this)
    fseek(wav, 44, SEEK_SET);

    // Create a new FLAC encoder instance
    FLAC__StreamEncoder *encoder = FLAC__stream_encoder_new();
    if (!encoder) { 
        fprintf(stderr, "Failed to create encoder\n"); 
        return 1; 
    }

    // Encoder configuration
    FLAC__stream_encoder_set_verify(encoder, true);         // Enable verification
    FLAC__stream_encoder_set_compression_level(encoder, 8); // Max compression
    FLAC__stream_encoder_set_channels(encoder, 2);         // Stereo
    FLAC__stream_encoder_set_bits_per_sample(encoder, 16); // 16-bit samples
    FLAC__stream_encoder_set_sample_rate(encoder, 44100);  // 44.1 kHz sample rate

    // Initialize encoder to output to a file
    if (FLAC__stream_encoder_init_file(encoder, OUTPUT_FLAC, NULL, NULL) 
        != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
        fprintf(stderr, "Failed to initialize encoder\n");
        return 1;
    }

    // Buffers:
    // buffer16 -> holds raw 16-bit PCM samples read from WAV
    // buffer32 -> FLAC encoder expects 32-bit integers
    int16_t buffer16[1024*2];      // 1024 frames * 2 channels
    FLAC__int32 buffer32[1024*2];  // Convert 16-bit -> 32-bit

    size_t read;
    // Read WAV file in chunks
    while ((read = fread(buffer16, sizeof(int16_t)*2, 1024, wav)) > 0) {
        // Convert each 16-bit sample to 32-bit
        for (size_t i = 0; i < read*2; i++) {
            buffer32[i] = buffer16[i]; // Promote 16-bit to 32-bit for FLAC
        }
        // Encode the interleaved stereo buffer
        if (!FLAC__stream_encoder_process_interleaved(encoder, buffer32, read)) {
            fprintf(stderr, "Encoding error\n");
            return 1;
        }
    }

    // Finish encoding (flush any remaining data)
    FLAC__stream_encoder_finish(encoder);

    // Delete encoder instance and free resources
    FLAC__stream_encoder_delete(encoder);

    // Close input WAV file
    fclose(wav);

    printf("WAV -> FLAC encoding completed.\n");
    return 0;
}

