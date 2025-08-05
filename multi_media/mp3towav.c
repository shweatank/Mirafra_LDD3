#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input.mp3> <output.wav>\n", argv[0]);
        return -1;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    AVFormatContext *fmt_ctx = NULL;
    AVCodecContext *codec_ctx = NULL;
    AVCodec *decoder = NULL;
    SwrContext *swr = NULL;
    AVPacket *pkt = NULL;
    AVFrame *frame = NULL;

    int audio_stream_index;

    avformat_open_input(&fmt_ctx, input_filename, NULL, NULL);
    avformat_find_stream_info(fmt_ctx, NULL);

    audio_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    AVStream *audio_stream = fmt_ctx->streams[audio_stream_index];

    decoder = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    codec_ctx = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(codec_ctx, audio_stream->codecpar);
    avcodec_open2(codec_ctx, decoder, NULL);

    frame = av_frame_alloc();
    pkt = av_packet_alloc();

    swr = swr_alloc();
    av_opt_set_int(swr, "in_channel_layout",    codec_ctx->channel_layout, 0);
    av_opt_set_int(swr, "out_channel_layout",   AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr, "in_sample_rate",       codec_ctx->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate",      44100, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", codec_ctx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr);

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("fopen");
        return -1;
    }

    // Write basic WAV header placeholder
    int data_chunk_pos = 44;
    fwrite("RIFF\0\0\0\0WAVEfmt ", 1, 16, out);
    uint32_t fmt_size = 16;
    uint16_t format = 1; // PCM
    uint16_t channels = 2;
    uint32_t sample_rate = 44100;
    uint16_t bits_per_sample = 16;
    uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
    uint16_t block_align = channels * bits_per_sample / 8;
    fwrite(&fmt_size, 4, 1, out);
    fwrite(&format, 2, 1, out);
    fwrite(&channels, 2, 1, out);
    fwrite(&sample_rate, 4, 1, out);
    fwrite(&byte_rate, 4, 1, out);
    fwrite(&block_align, 2, 1, out);
    fwrite(&bits_per_sample, 2, 1, out);
    fwrite("data\0\0\0\0", 1, 8, out); // Data header placeholder

    int total_data_bytes = 0;

    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == audio_stream_index) {
            avcodec_send_packet(codec_ctx, pkt);
            while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                uint8_t *output;
                int out_samples = av_rescale_rnd(swr_get_delay(swr, codec_ctx->sample_rate) + frame->nb_samples, 44100, codec_ctx->sample_rate, AV_ROUND_UP);
                av_samples_alloc(&output, NULL, 2, out_samples, AV_SAMPLE_FMT_S16, 0);
                int converted = swr_convert(swr, &output, out_samples, (const uint8_t **)frame->extended_data, frame->nb_samples);
                int out_bytes = av_samples_get_buffer_size(NULL, 2, converted, AV_SAMPLE_FMT_S16, 0);
                fwrite(output, 1, out_bytes, out);
                total_data_bytes += out_bytes;
                av_freep(&output);
            }
        }
        av_packet_unref(pkt);
    }

    // Update WAV file sizes
    fseek(out, 4, SEEK_SET);
    uint32_t file_size = total_data_bytes + 36;
    fwrite(&file_size, 4, 1, out);
    fseek(out, 40, SEEK_SET);
    fwrite(&total_data_bytes, 4, 1, out);
    fclose(out);

    av_packet_free(&pkt);
    av_frame_free(&frame);
    swr_free(&swr);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    printf("Converted %s -> %s\n", input_filename, output_filename);
    return 0;
}

