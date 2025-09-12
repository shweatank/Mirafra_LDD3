#include <FLAC/stream_encoder.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define INPUT_WAV "recorded.wav"
#define OUTPUT_FLAC "output.flac"

int main() {
    FILE *wav = fopen(INPUT_WAV, "rb");
    if (!wav) { perror("WAV open"); return 1; }

    fseek(wav, 44, SEEK_SET); // Skip WAV header

    FLAC__StreamEncoder *encoder = FLAC__stream_encoder_new();
    if (!encoder) { fprintf(stderr, "Failed to create encoder\n"); return 1; }

    FLAC__stream_encoder_set_verify(encoder, true);
    FLAC__stream_encoder_set_compression_level(encoder, 8);
    FLAC__stream_encoder_set_channels(encoder, 2); // stereo
    FLAC__stream_encoder_set_bits_per_sample(encoder, 16);
    FLAC__stream_encoder_set_sample_rate(encoder, 44100);

    if (FLAC__stream_encoder_init_file(encoder, OUTPUT_FLAC, NULL, NULL) 
        != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
        fprintf(stderr, "Failed to initialize encoder\n");
        return 1;
    }

    int16_t buffer16[1024*2];      // read 16-bit samples
    FLAC__int32 buffer32[1024*2];  // convert to 32-bit samples

    size_t read;
    while ((read = fread(buffer16, sizeof(int16_t)*2, 1024, wav)) > 0) {
        // Convert each sample
        for (size_t i = 0; i < read*2; i++) {
            buffer32[i] = buffer16[i]; // promote 16-bit -> 32-bit
        }
        if (!FLAC__stream_encoder_process_interleaved(encoder, buffer32, read)) {
            fprintf(stderr, "Encoding error\n");
            return 1;
        }
    }

    FLAC__stream_encoder_finish(encoder);
    FLAC__stream_encoder_delete(encoder);
    fclose(wav);

    printf("WAV -> FLAC encoding completed.\n");
    return 0;
}

