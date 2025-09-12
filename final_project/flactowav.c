#include <FLAC/stream_decoder.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define INPUT_FLAC  "recv_audio.flac"
#define OUTPUT_WAV  "decoded.wav"

FILE *wav;
unsigned sample_rate = 0;
unsigned channels = 0;
unsigned bps = 0;   // bits per sample
long data_size = 0; // PCM data size tracker

// Write placeholder WAV header
void write_wav_header(FILE *f, unsigned sample_rate, unsigned channels, unsigned bits_per_sample) {
    unsigned byte_rate = sample_rate * channels * bits_per_sample / 8;
    unsigned block_align = channels * bits_per_sample / 8;

    // RIFF chunk
    fwrite("RIFF", 1, 4, f);
    uint32_t chunk_size = 0; // placeholder, will fix later
    fwrite(&chunk_size, 4, 1, f);
    fwrite("WAVE", 1, 4, f);

    // fmt subchunk
    fwrite("fmt ", 1, 4, f);
    uint32_t subchunk1_size = 16;
    fwrite(&subchunk1_size, 4, 1, f);
    uint16_t audio_format = 1; // PCM
    fwrite(&audio_format, 2, 1, f);
    uint16_t num_channels = channels;
    fwrite(&num_channels, 2, 1, f);
    fwrite(&sample_rate, 4, 1, f);
    fwrite(&byte_rate, 4, 1, f);
    fwrite(&block_align, 2, 1, f);
    fwrite(&bits_per_sample, 2, 1, f);

    // data subchunk
    fwrite("data", 1, 4, f);
    uint32_t data_chunk_size = 0; // placeholder
    fwrite(&data_chunk_size, 4, 1, f);
}

// Fix header sizes after writing data
void fix_wav_header(FILE *f, long data_size) {
    fseek(f, 4, SEEK_SET);
    uint32_t chunk_size = 36 + data_size;
    fwrite(&chunk_size, 4, 1, f);

    fseek(f, 40, SEEK_SET);
    uint32_t data_chunk_size = data_size;
    fwrite(&data_chunk_size, 4, 1, f);
}

FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder,
                                              const FLAC__Frame *frame,
                                              const FLAC__int32 * const buffer[],
                                              void *client_data)
{
    int blocksize = frame->header.blocksize;
    int ch = frame->header.channels;

    for (int i = 0; i < blocksize; i++) {
        for (int c = 0; c < ch; c++) {
            int16_t sample = (int16_t)buffer[c][i]; // downcast to 16-bit
            fwrite(&sample, sizeof(int16_t), 1, wav);
            data_size += sizeof(int16_t);
        }
    }
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void metadata_callback(const FLAC__StreamDecoder *decoder,
                       const FLAC__StreamMetadata *metadata,
                       void *client_data)
{
    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        sample_rate = metadata->data.stream_info.sample_rate;
        channels = metadata->data.stream_info.channels;
        bps = metadata->data.stream_info.bits_per_sample;

        // Write placeholder header now
        write_wav_header(wav, sample_rate, channels, bps);
    }
}

void error_callback(const FLAC__StreamDecoder *decoder,
                    FLAC__StreamDecoderErrorStatus status,
                    void *client_data)
{
    fprintf(stderr, "Decoding error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

int main() {
    wav = fopen(OUTPUT_WAV, "wb+");
    if (!wav) { perror("WAV open"); return 1; }

    FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new();
    if (!decoder) { fprintf(stderr, "Failed to create decoder\n"); return 1; }

    FLAC__stream_decoder_init_file(decoder, INPUT_FLAC,
                                   write_callback,
                                   metadata_callback,
                                   error_callback,
                                   NULL);

    FLAC__stream_decoder_process_until_end_of_stream(decoder);

    // Finalize header
    fix_wav_header(wav, data_size);

    FLAC__stream_decoder_finish(decoder);
    FLAC__stream_decoder_delete(decoder);
    fclose(wav);

    printf("FLAC -> WAV decoding completed.\n");
    return 0;
}

