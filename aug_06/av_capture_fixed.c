
// av_capture_fixed.c
// Capture video from /dev/video0 (V4L2) and audio from ALSA, encode using H264/AAC, and mux into MP4

#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OUTPUT_FILE "output.mp4"
#define WIDTH 640
#define HEIGHT 480
#define FPS 25
#define SAMPLE_RATE 44100
#define CHANNELS 2
#define DURATION_SEC 10

AVFormatContext *video_fmt_ctx = NULL, *audio_fmt_ctx = NULL, *out_fmt_ctx = NULL;
AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx = NULL;
AVCodecContext *video_enc_ctx = NULL, *audio_enc_ctx = NULL;
AVStream *video_out_stream = NULL, *audio_out_stream = NULL;
int video_index = -1, audio_index = -1;
struct SwsContext *sws_ctx = NULL;
struct SwrContext *swr_ctx = NULL;

int init_input_devices() {
    AVInputFormat *video_input_fmt = (AVInputFormat *)av_find_input_format("v4l2");
    if (avformat_open_input(&video_fmt_ctx, "/dev/video0", video_input_fmt, NULL) != 0)
        return -1;
    if (avformat_find_stream_info(video_fmt_ctx, NULL) < 0)
        return -1;

    AVInputFormat *audio_input_fmt = (AVInputFormat *)av_find_input_format("alsa");
    if (avformat_open_input(&audio_fmt_ctx, "default", audio_input_fmt, NULL) != 0)
        return -1;
    if (avformat_find_stream_info(audio_fmt_ctx, NULL) < 0)
        return -1;

    return 0;
}

int init_output_file() {
    avformat_alloc_output_context2(&out_fmt_ctx, NULL, "mp4", OUTPUT_FILE);
    if (!out_fmt_ctx) return -1;

    AVCodec *video_encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    video_out_stream = avformat_new_stream(out_fmt_ctx, video_encoder);
    video_enc_ctx = avcodec_alloc_context3(video_encoder);
    video_enc_ctx->height = HEIGHT;
    video_enc_ctx->width = WIDTH;
    video_enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    video_enc_ctx->time_base = (AVRational){1, FPS};
    video_out_stream->time_base = video_enc_ctx->time_base;
    avcodec_open2(video_enc_ctx, video_encoder, NULL);
    avcodec_parameters_from_context(video_out_stream->codecpar, video_enc_ctx);

    AVCodec *audio_encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
    audio_out_stream = avformat_new_stream(out_fmt_ctx, audio_encoder);
    audio_enc_ctx = avcodec_alloc_context3(audio_encoder);
    audio_enc_ctx->sample_rate = SAMPLE_RATE;
    audio_enc_ctx->channel_layout = AV_CH_LAYOUT_STEREO;
    audio_enc_ctx->channels = av_get_channel_layout_nb_channels(audio_enc_ctx->channel_layout);
    audio_enc_ctx->sample_fmt = audio_encoder->sample_fmts[0];
    audio_enc_ctx->bit_rate = 128000;
    audio_enc_ctx->time_base = (AVRational){1, SAMPLE_RATE};
    audio_out_stream->time_base = audio_enc_ctx->time_base;
    avcodec_open2(audio_enc_ctx, audio_encoder, NULL);
    avcodec_parameters_from_context(audio_out_stream->codecpar, audio_enc_ctx);

    if (!(out_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&out_fmt_ctx->pb, OUTPUT_FILE, AVIO_FLAG_WRITE) < 0)
            return -1;
    }

    if (avformat_write_header(out_fmt_ctx, NULL) < 0)
        return -1;

    return 0;
}

void cleanup() {
    avcodec_free_context(&video_enc_ctx);
    avcodec_free_context(&audio_enc_ctx);
    avformat_close_input(&video_fmt_ctx);
    avformat_close_input(&audio_fmt_ctx);
    avformat_free_context(out_fmt_ctx);
}

int main() {
    avdevice_register_all();
    avformat_network_init();

    if (init_input_devices() < 0) {
        fprintf(stderr, "Failed to open input devices.\n");
        return -1;
    }

    if (init_output_file() < 0) {
        fprintf(stderr, "Failed to set up output.\n");
        return -1;
    }

    int64_t start_time = av_gettime();
    AVPacket pkt;
    while ((av_gettime() - start_time) / 1000000 < DURATION_SEC) {
        if (av_read_frame(video_fmt_ctx, &pkt) >= 0 && pkt.stream_index == 0) {
            pkt.stream_index = video_out_stream->index;
            pkt.pts = pkt.dts = av_rescale_q(pkt.pts, video_fmt_ctx->streams[0]->time_base, video_out_stream->time_base);
            pkt.duration = av_rescale_q(pkt.duration, video_fmt_ctx->streams[0]->time_base, video_out_stream->time_base);
            av_interleaved_write_frame(out_fmt_ctx, &pkt);
            av_packet_unref(&pkt);
        }

        if (av_read_frame(audio_fmt_ctx, &pkt) >= 0 && pkt.stream_index == 0) {
            pkt.stream_index = audio_out_stream->index;
            pkt.pts = pkt.dts = av_rescale_q(pkt.pts, audio_fmt_ctx->streams[0]->time_base, audio_out_stream->time_base);
            pkt.duration = av_rescale_q(pkt.duration, audio_fmt_ctx->streams[0]->time_base, audio_out_stream->time_base);
            av_interleaved_write_frame(out_fmt_ctx, &pkt);
            av_packet_unref(&pkt);
        }
    }

    av_write_trailer(out_fmt_ctx);
    cleanup();
    printf("Capture complete: %s\n", OUTPUT_FILE);
    return 0;
}
