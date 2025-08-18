#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>

#define STREAM_FRAME_RATE 25
#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P
#define STREAM_WIDTH 640
#define STREAM_HEIGHT 480
#define AUDIO_SAMPLE_RATE 44100

int tcp_connect(const char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) { perror("socket"); return -1; }
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return -1;
    }
    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }
    return sock;
}

int write_packet(void *opaque, uint8_t *buf, int buf_size) {
    int sockfd = *(int*)opaque;
    int sent = 0;
    while(sent < buf_size) {
        int n = send(sockfd, buf + sent, buf_size - sent, 0);
        if(n <= 0) return -1;
        sent += n;
    }
    return buf_size;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <dest_ip> <dest_port>\n", argv[0]);
        return -1;
    }
    const char *dest_ip = argv[1];
    int dest_port = atoi(argv[2]);

    avdevice_register_all();
    avformat_network_init();

    // Cast away const to avoid warnings
    AVInputFormat *video_input_format = (AVInputFormat *)av_find_input_format("v4l2");
    AVFormatContext *video_input_ctx = NULL;
    if(avformat_open_input(&video_input_ctx, "/dev/video0", video_input_format, NULL) < 0) {
        fprintf(stderr, "Cannot open video device\n");
        return -1;
    }
    avformat_find_stream_info(video_input_ctx, NULL);

    AVInputFormat *audio_input_format = (AVInputFormat *)av_find_input_format("alsa");
    AVFormatContext *audio_input_ctx = NULL;
    if(avformat_open_input(&audio_input_ctx, "default", audio_input_format, NULL) < 0) {
        fprintf(stderr, "Cannot open audio device\n");
        return -1;
    }
    avformat_find_stream_info(audio_input_ctx, NULL);

    AVStream *video_in_stream = video_input_ctx->streams[0];
    AVCodec *video_dec = (AVCodec *)avcodec_find_decoder(video_in_stream->codecpar->codec_id);
    AVCodecContext *video_dec_ctx = avcodec_alloc_context3(video_dec);
    avcodec_parameters_to_context(video_dec_ctx, video_in_stream->codecpar);
    avcodec_open2(video_dec_ctx, video_dec, NULL);

    AVCodec *video_enc = (AVCodec *)avcodec_find_encoder(AV_CODEC_ID_H264);
    AVCodecContext *video_enc_ctx = avcodec_alloc_context3(video_enc);
    video_enc_ctx->width = STREAM_WIDTH;
    video_enc_ctx->height = STREAM_HEIGHT;
    video_enc_ctx->pix_fmt = STREAM_PIX_FMT;
    video_enc_ctx->time_base = (AVRational){1, STREAM_FRAME_RATE};
    video_enc_ctx->framerate = (AVRational){STREAM_FRAME_RATE, 1};
    video_enc_ctx->bit_rate = 800000;
    video_enc_ctx->gop_size = 12;
    av_opt_set(video_enc_ctx->priv_data, "preset", "ultrafast", 0);
    if(avcodec_open2(video_enc_ctx, video_enc, NULL) < 0) {
        fprintf(stderr, "Cannot open video encoder\n");
        return -1;
    }

    AVStream *audio_in_stream = audio_input_ctx->streams[0];
    AVCodec *audio_dec = (AVCodec *)avcodec_find_decoder(audio_in_stream->codecpar->codec_id);
    AVCodecContext *audio_dec_ctx = avcodec_alloc_context3(audio_dec);
    avcodec_parameters_to_context(audio_dec_ctx, audio_in_stream->codecpar);
    avcodec_open2(audio_dec_ctx, audio_dec, NULL);

    AVCodec *audio_enc = (AVCodec *)avcodec_find_encoder(AV_CODEC_ID_AAC);
    AVCodecContext *audio_enc_ctx = avcodec_alloc_context3(audio_enc);

    // Fix: use deprecated channel_layout field + av_get_channel_layout_nb_channels()
    audio_enc_ctx->channel_layout = AV_CH_LAYOUT_STEREO;
    audio_enc_ctx->channels = av_get_channel_layout_nb_channels(audio_enc_ctx->channel_layout);

    audio_enc_ctx->sample_rate = AUDIO_SAMPLE_RATE;
    audio_enc_ctx->sample_fmt = audio_enc->sample_fmts[0];
    audio_enc_ctx->time_base = (AVRational){1, audio_enc_ctx->sample_rate};
    audio_enc_ctx->bit_rate = 64000;
    if(avcodec_open2(audio_enc_ctx, audio_enc, NULL) < 0) {
        fprintf(stderr, "Cannot open audio encoder\n");
        return -1;
    }

    SwrContext *swr_ctx = swr_alloc();
    av_opt_set_int(swr_ctx, "in_channel_layout", audio_dec_ctx->channel_layout, 0);
    av_opt_set_int(swr_ctx, "out_channel_layout", audio_enc_ctx->channel_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", audio_dec_ctx->sample_rate, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", audio_enc_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", audio_dec_ctx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", audio_enc_ctx->sample_fmt, 0);
    swr_init(swr_ctx);

    struct SwsContext *sws_ctx = sws_getContext(
        video_dec_ctx->width, video_dec_ctx->height, video_dec_ctx->pix_fmt,
        video_enc_ctx->width, video_enc_ctx->height, video_enc_ctx->pix_fmt,
        SWS_BILINEAR, NULL, NULL, NULL);

    AVFormatContext *output_ctx = NULL;
    if(avformat_alloc_output_context2(&output_ctx, NULL, "mpegts", NULL) < 0) {
        fprintf(stderr,"Could not allocate output context\n");
        return -1;
    }

    AVStream *out_video_stream = avformat_new_stream(output_ctx, NULL);
    avcodec_parameters_from_context(out_video_stream->codecpar, video_enc_ctx);
    out_video_stream->time_base = video_enc_ctx->time_base;

    AVStream *out_audio_stream = avformat_new_stream(output_ctx, NULL);
    avcodec_parameters_from_context(out_audio_stream->codecpar, audio_enc_ctx);
    out_audio_stream->time_base = audio_enc_ctx->time_base;

    int tcp_sock = tcp_connect(dest_ip, dest_port);
    if(tcp_sock < 0) {
        fprintf(stderr,"Cannot connect to %s:%d\n", dest_ip, dest_port);
        return -1;
    }

    unsigned char *avio_ctx_buffer = av_malloc(32768);
    AVIOContext *avio_ctx = avio_alloc_context(avio_ctx_buffer, 32768, 1, &tcp_sock, NULL, write_packet, NULL);
    output_ctx->pb = avio_ctx;

    if(avformat_write_header(output_ctx, NULL) < 0) {
        fprintf(stderr,"Error writing header\n");
        return -1;
    }

    printf("Streaming to %s:%d\n", dest_ip, dest_port);

    AVPacket packet;
    av_packet_unref(&packet);

    AVFrame *video_frame = av_frame_alloc();
    AVFrame *video_frame_scaled = av_frame_alloc();
    video_frame_scaled->format = video_enc_ctx->pix_fmt;
    video_frame_scaled->width = video_enc_ctx->width;
    video_frame_scaled->height = video_enc_ctx->height;
    av_frame_get_buffer(video_frame_scaled, 32);

    AVFrame *audio_frame = av_frame_alloc();

    int video_finished = 0, audio_finished = 0;

    while(1) {
        if(!video_finished) {
            if(av_read_frame(video_input_ctx, &packet) == 0 && packet.stream_index == 0) {
                if(avcodec_send_packet(video_dec_ctx, &packet) >= 0) {
                    while(avcodec_receive_frame(video_dec_ctx, video_frame) == 0) {
                        sws_scale(sws_ctx, (const uint8_t * const*)video_frame->data, video_frame->linesize, 0,
                            video_dec_ctx->height, video_frame_scaled->data, video_frame_scaled->linesize);
                        video_frame_scaled->pts = video_frame->pts;

                        if(avcodec_send_frame(video_enc_ctx, video_frame_scaled) >= 0) {
                            AVPacket enc_pkt;
                            av_init_packet(&enc_pkt);
                            enc_pkt.data = NULL;
                            enc_pkt.size = 0;
                            while(avcodec_receive_packet(video_enc_ctx, &enc_pkt) == 0) {
                                enc_pkt.stream_index = out_video_stream->index;
                                av_interleaved_write_frame(output_ctx, &enc_pkt);
                                av_packet_unref(&enc_pkt);
                            }
                        }
                    }
                }
                av_packet_unref(&packet);
            } else {
                video_finished = 1;
            }
        }

        if(!audio_finished) {
            if(av_read_frame(audio_input_ctx, &packet) == 0 && packet.stream_index == 0) {
                if(avcodec_send_packet(audio_dec_ctx, &packet) >= 0) {
                    while(avcodec_receive_frame(audio_dec_ctx, audio_frame) == 0) {
                        AVFrame *resampled_frame = av_frame_alloc();
                        resampled_frame->channel_layout = audio_enc_ctx->channel_layout;
                        resampled_frame->format = audio_enc_ctx->sample_fmt;
                        resampled_frame->sample_rate = audio_enc_ctx->sample_rate;
                        resampled_frame->nb_samples = audio_frame->nb_samples;

                        av_frame_get_buffer(resampled_frame, 0);

                        swr_convert(swr_ctx, resampled_frame->data, resampled_frame->nb_samples,
                                    (const uint8_t **)audio_frame->data, audio_frame->nb_samples);

                        resampled_frame->pts = audio_frame->pts;

                        if(avcodec_send_frame(audio_enc_ctx, resampled_frame) >= 0) {
                            AVPacket enc_pkt;
                            av_init_packet(&enc_pkt);
                            enc_pkt.data = NULL;
                            enc_pkt.size = 0;
                            while(avcodec_receive_packet(audio_enc_ctx, &enc_pkt) == 0) {
                                enc_pkt.stream_index = out_audio_stream->index;
                                av_interleaved_write_frame(output_ctx, &enc_pkt);
                                av_packet_unref(&enc_pkt);
                            }
                        }
                        av_frame_free(&resampled_frame);
                    }
                }
                av_packet_unref(&packet);
            } else {
                audio_finished = 1;
            }
        }

        if(video_finished && audio_finished) break;
    }

    av_write_trailer(output_ctx);

    av_frame_free(&video_frame);
    av_frame_free(&video_frame_scaled);
    av_frame_free(&audio_frame);

    avcodec_free_context(&video_dec_ctx);
    avcodec_free_context(&video_enc_ctx);
    avcodec_free_context(&audio_dec_ctx);
    avcodec_free_context(&audio_enc_ctx);

    avformat_close_input(&video_input_ctx);
    avformat_close_input(&audio_input_ctx);

    sws_freeContext(sws_ctx);
    swr_free(&swr_ctx);

    avformat_free_context(output_ctx);

    av_free(avio_ctx_buffer);
    avio_context_free(&avio_ctx);
    close(tcp_sock);

    avformat_network_deinit();

    return 0;
}

