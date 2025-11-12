// Include FLAC decoding library
#include <FLAC/stream_decoder.h>   // FLAC stream decoding APIs
#include <stdio.h>                 // printf, perror, FILE
#include <stdlib.h>                // exit, malloc, free
#include <stdint.h>                // fixed width integer types
#include <string.h>                // memset, strcpy, etc.

// Input/output file names
#define INPUT_FLAC  "recv_audio.flac" // FLAC file received from TCP client
#define OUTPUT_WAV  "decoded.wav"     // WAV file to be generated

// Global variables
FILE *wav;                // File pointer for WAV output
unsigned sample_rate = 0; // Sample rate of audio (filled from FLAC metadata)
unsigned channels = 0;    // Number of audio channels (filled from FLAC metadata)
unsigned bps = 0;         // Bits per sample (filled from FLAC metadata)
long data_size = 0;       // Total PCM data size written, for WAV header

// Function to write a **placeholder WAV header** at the beginning of the file
void write_wav_header(FILE *f, unsigned sample_rate, unsigned channels, unsigned bits_per_sample) {
    unsigned byte_rate = sample_rate * channels * bits_per_sample / 8;   // bytes per second
    unsigned block_align = channels * bits_per_sample / 8;               // bytes per frame

    // --- RIFF chunk ---
    fwrite("RIFF", 1, 4, f);            // Chunk ID "RIFF"
    uint32_t chunk_size = 0;             // Placeholder for total file size
    fwrite(&chunk_size, 4, 1, f);        // Will fix later
    fwrite("WAVE", 1, 4, f);            // Format "WAVE"

    // --- fmt subchunk ---
    fwrite("fmt ", 1, 4, f);            // Subchunk ID "fmt "
    uint32_t subchunk1_size = 16;       // PCM subchunk size
    fwrite(&subchunk1_size, 4, 1, f);
    uint16_t audio_format = 1;          // PCM format
    fwrite(&audio_format, 2, 1, f);
    uint16_t num_channels = channels;   // Number of channels
    fwrite(&num_channels, 2, 1, f);
    fwrite(&sample_rate, 4, 1, f);      // Sample rate
    fwrite(&byte_rate, 4, 1, f);        // Byte rate
    fwrite(&block_align, 2, 1, f);      // Block align
    fwrite(&bits_per_sample, 2, 1, f);  // Bits per sample

    // --- data subchunk ---
    fwrite("data", 1, 4, f);            // Subchunk ID "data"
    uint32_t data_chunk_size = 0;       // Placeholder for data size
    fwrite(&data_chunk_size, 4, 1, f);
}

// Function to **fix the WAV header** after all PCM data has been written
void fix_wav_header(FILE *f, long data_size) {
    fseek(f, 4, SEEK_SET);                     // Go to RIFF chunk size
    uint32_t chunk_size = 36 + data_size;      // RIFF size = 36 + data size
    fwrite(&chunk_size, 4, 1, f);

    fseek(f, 40, SEEK_SET);                    // Go to data subchunk size
    uint32_t data_chunk_size = data_size;
    fwrite(&data_chunk_size, 4, 1, f);
}

// FLAC write callback, called for each decoded audio block
FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder,
                                              const FLAC__Frame *frame,
                                              const FLAC__int32 * const buffer[],
                                              void *client_data)
{
    int blocksize = frame->header.blocksize;   // Number of samples per channel in this frame
    int ch = frame->header.channels;           // Number of channels

    // Write samples to WAV file, interleaved
    for (int i = 0; i < blocksize; i++) {
        for (int c = 0; c < ch; c++) {
            int16_t sample = (int16_t)buffer[c][i]; // Convert FLAC 32-bit -> 16-bit PCM
            fwrite(&sample, sizeof(int16_t), 1, wav);
            data_size += sizeof(int16_t);           // Keep track of PCM data size
        }
    }
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE; // Continue decoding
}

// Metadata callback, called once at the start of stream
void metadata_callback(const FLAC__StreamDecoder *decoder,
                       const FLAC__StreamMetadata *metadata,
                       void *client_data)
{
    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        sample_rate = metadata->data.stream_info.sample_rate;  // Fill sample rate
        channels = metadata->data.stream_info.channels;        // Fill number of channels
        bps = metadata->data.stream_info.bits_per_sample;      // Fill bits per sample

        // Write placeholder WAV header at beginning
        write_wav_header(wav, sample_rate, channels, bps);
    }
}

// Error callback, called if decoding fails
void error_callback(const FLAC__StreamDecoder *decoder,
                    FLAC__StreamDecoderErrorStatus status,
                    void *client_data)
{
    fprintf(stderr, "Decoding error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

int main() {
    // Open WAV file for writing (wb+ allows reading/updating header)
    wav = fopen(OUTPUT_WAV, "wb+");
    if (!wav) { perror("WAV open"); return 1; }

    // Create new FLAC decoder
    FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new();
    if (!decoder) { fprintf(stderr, "Failed to create decoder\n"); return 1; }

    // Initialize decoder to read from input FLAC file with callbacks
    FLAC__stream_decoder_init_file(decoder, INPUT_FLAC,
                                   write_callback,      // Called per frame
                                   metadata_callback,   // Called once for metadata
                                   error_callback,      // Called on errors
                                   NULL);               // Client data (unused)

    // Process FLAC stream until end-of-file
    FLAC__stream_decoder_process_until_end_of_stream(decoder);

    // Fix WAV header with correct sizes
    fix_wav_header(wav, data_size);

    // Clean up decoder
    FLAC__stream_decoder_finish(decoder);
    FLAC__stream_decoder_delete(decoder);

    // Close WAV file
    fclose(wav);

    printf("FLAC -> WAV decoding completed.\n");
    return 0;
}

