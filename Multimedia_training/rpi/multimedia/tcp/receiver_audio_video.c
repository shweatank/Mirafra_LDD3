// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <alsa/asoundlib.h>


#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#define SDL_AUDIO_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <tcp://ip:port>\n", argv[0]);
        return -1;
    }

    avformat_network_init();

    AVFormatContext *fmt_ctx = NULL;
    if (avformat_open_input(&fmt_ctx, argv[1], NULL, NULL) != 0) {
        fprintf(stderr, "Could not open input\n");
        return -1;
    }

    avformat_find_stream_info(fmt_ctx, NULL);

    int video_stream_index = -1, audio_stream_index = -1;
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) video_stream_index = i;
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) audio_stream_index = i;
    }

    if (video_stream_index == -1 || audio_stream_index == -1) {
        fprintf(stderr, "No video or audio stream found\n");
        return -1;
    }

    AVCodecContext *video_dec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(video_dec_ctx, fmt_ctx->streams[video_stream_index]->codecpar);
    AVCodec *video_decoder = avcodec_find_decoder(video_dec_ctx->codec_id);
    avcodec_open2(video_dec_ctx, video_decoder, NULL);

    AVCodecContext *audio_dec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(audio_dec_ctx, fmt_ctx->streams[audio_stream_index]->codecpar);
    AVCodec *audio_decoder = avcodec_find_decoder(audio_dec_ctx->codec_id);
    avcodec_open2(audio_dec_ctx, audio_decoder, NULL);

    // Setup SDL video
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Video",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        video_dec_ctx->width, video_dec_ctx->height, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING,
        video_dec_ctx->width, video_dec_ctx->height);

    struct SwsContext *sws_ctx = sws_getContext(
        video_dec_ctx->width, video_dec_ctx->height, video_dec_ctx->pix_fmt,
        video_dec_ctx->width, video_dec_ctx->height, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, NULL, NULL, NULL);

    // Setup ALSA audio output
    snd_pcm_t *pcm_handle;
    snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(pcm_handle,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        audio_dec_ctx->channels,
        audio_dec_ctx->sample_rate,
        1, 500000); // 0.5 sec latency

    struct SwrContext *swr_ctx = swr_alloc_set_opts(NULL,
        av_get_default_channel_layout(audio_dec_ctx->channels),
        AV_SAMPLE_FMT_S16,
        audio_dec_ctx->sample_rate,
        av_get_default_channel_layout(audio_dec_ctx->channels),
        audio_dec_ctx->sample_fmt,
        audio_dec_ctx->sample_rate,
        0, NULL);
    swr_init(swr_ctx);

    AVPacket packet;
    AVFrame *frame = av_frame_alloc();
    AVFrame *audio_frame = av_frame_alloc();

    while (av_read_frame(fmt_ctx, &packet) >= 0) {
        if (packet.stream_index == video_stream_index) {
            avcodec_send_packet(video_dec_ctx, &packet);
            while (avcodec_receive_frame(video_dec_ctx, frame) == 0) {
                uint8_t *data[3];
                int linesize[3];
                SDL_UpdateYUVTexture(texture, NULL,
                    frame->data[0], frame->linesize[0],
                    frame->data[1], frame->linesize[1],
                    frame->data[2], frame->linesize[2]);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }
        } else if (packet.stream_index == audio_stream_index) {
            avcodec_send_packet(audio_dec_ctx, &packet);
            while (avcodec_receive_frame(audio_dec_ctx, audio_frame) == 0) {
                uint8_t *out_data;
                int out_linesize;
                int out_nb_samples = swr_convert(swr_ctx, &out_data, audio_frame->nb_samples,
                    (const uint8_t **)audio_frame->data, audio_frame->nb_samples);

                snd_pcm_writei(pcm_handle, out_data, out_nb_samples);
            }
        }
        av_packet_unref(&packet);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                goto end;
            }
        }
    }

end:
    av_frame_free(&frame);
    av_frame_free(&audio_frame);
    avcodec_free_context(&video_dec_ctx);
    avcodec_free_context(&audio_dec_ctx);
    avformat_close_input(&fmt_ctx);
    snd_pcm_close(pcm_handle);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

