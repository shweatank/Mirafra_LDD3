// pi_player.c
// Minimal multi-threaded player: FFmpeg (decode) + ALSA (audio) + /dev/ili9225 (video)
// NOTE: Adapt device paths and settings to your environment.

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

#include <alsa/asoundlib.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>

#define ILI9225_DEV "/dev/ili9225"   // change if your driver exposes another path
#define ILI9225_W 176
#define ILI9225_H 220

#define AUDIO_QUEUE_MAX 200
#define VIDEO_QUEUE_MAX 200

typedef struct PacketNode {
    AVPacket *pkt;
    struct PacketNode *next;
} PacketNode;

typedef struct PacketQueue {
    PacketNode *first, *last;
    int nb_packets;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int abort_request;
    int max_packets;
} PacketQueue;

static void packet_queue_init(PacketQueue *q, int max_packets) {
    q->first = q->last = NULL;
    q->nb_packets = 0;
    q->abort_request = 0;
    q->max_packets = max_packets;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

static void packet_queue_flush(PacketQueue *q) {
    pthread_mutex_lock(&q->mutex);
    PacketNode *n = q->first;
    while (n) {
        PacketNode *tmp = n->next;
        if (n->pkt) av_packet_free(&n->pkt);
        free(n);
        n = tmp;
    }
    q->first = q->last = NULL;
    q->nb_packets = 0;
    pthread_mutex_unlock(&q->mutex);
}

static void packet_queue_destroy(PacketQueue *q) {
    packet_queue_flush(q);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

static int packet_queue_put(PacketQueue *q, AVPacket *pkt) {
    if (q->abort_request) return -1;
    PacketNode *node = malloc(sizeof(PacketNode));
    if (!node) return -1;
    node->pkt = av_packet_alloc();
    if (!node->pkt) { free(node); return -1; }
    if (av_packet_ref(node->pkt, pkt) < 0) {
        av_packet_free(&node->pkt);
        free(node);
        return -1;
    }
    node->next = NULL;

    pthread_mutex_lock(&q->mutex);
    while (q->nb_packets >= q->max_packets && !q->abort_request) {
        // wait until consumer drains queue
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    if (q->abort_request) {
        pthread_mutex_unlock(&q->mutex);
        av_packet_free(&node->pkt);
        free(node);
        return -1;
    }
    if (!q->last) q->first = node;
    else q->last->next = node;
    q->last = node;
    q->nb_packets++;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block) {
    int ret = 0;
    pthread_mutex_lock(&q->mutex);
    for (;;) {
        if (q->abort_request) { ret = -1; break; }
        PacketNode *node = q->first;
        if (node) {
            q->first = node->next;
            if (!q->first) q->last = NULL;
            q->nb_packets--;
            if (pkt) {
                if (av_packet_ref(pkt, node->pkt) < 0) ret = -1;
            }
            av_packet_free(&node->pkt);
            free(node);
            pthread_cond_signal(&q->cond); // in case producer is waiting for space
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&q->cond, &q->mutex);
        }
    }
    pthread_mutex_unlock(&q->mutex);
    return ret;
}

static void packet_queue_abort(PacketQueue *q) {
    pthread_mutex_lock(&q->mutex);
    q->abort_request = 1;
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

/* Global shared context */
typedef struct PlayerState {
    AVFormatContext *fmt_ctx;

    int audio_stream_idx;
    int video_stream_idx;

    AVCodecContext *audio_dec_ctx;
    AVCodecContext *video_dec_ctx;

    PacketQueue audioq;
    PacketQueue videoq;

    pthread_t audio_tid;
    pthread_t video_tid;

    // ALSA
    snd_pcm_t *pcm_handle;
    snd_pcm_format_t alsa_format;
    int alsa_channels;
    int alsa_samplerate;

    // audio clock tracking
    pthread_mutex_t audio_clock_mutex;
    double audio_clock_seconds; // amount of audio played (seconds)

    // device for lcd
    int ili_fd;

    int quit;
} PlayerState;

static PlayerState *global_state = NULL;

/* utility: get current time in seconds (monotonic) */
static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* Audio decoding + playback thread */
static void *audio_thread(void *arg) {
    PlayerState *ps = (PlayerState *)arg;
    AVPacket pkt;
    av_init_packet(&pkt);

    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Error allocating audio frame\n");
        return NULL;
    }

    struct SwrContext *swr = NULL;
    uint8_t **conv_samples = NULL;
    int conv_linesize;
    int max_out_samples = 0;

    while (!ps->quit) {
        int r = packet_queue_get(&ps->audioq, &pkt, 1);
        if (r < 0) break;
        if (r == 0) continue; // no packet

        // send to decoder
        if (avcodec_send_packet(ps->audio_dec_ctx, &pkt) < 0) {
            av_packet_unref(&pkt);
            continue;
        }
        av_packet_unref(&pkt);

        // receive frames
        while (avcodec_receive_frame(ps->audio_dec_ctx, frame) == 0) {
            // Initialize resampler if needed
            enum AVSampleFormat in_fmt = frame->format;
            int in_rate = frame->sample_rate;
            int in_channels = av_get_channel_layout_nb_channels(frame->channel_layout ? frame->channel_layout : av_get_default_channel_layout(frame->channels));
            if (!in_channels) in_channels = frame->channels;

            // target: signed 16-bit little endian interleaved (SND_PCM_FORMAT_S16_LE)
            enum AVSampleFormat out_fmt = AV_SAMPLE_FMT_S16;
            int out_rate = ps->alsa_samplerate;
            int out_channels = ps->alsa_channels;
            uint64_t out_ch_layout = av_get_default_channel_layout(out_channels);

            if (!swr) {
                swr = swr_alloc_set_opts(NULL,
                                         out_ch_layout, out_fmt, out_rate,
                                         frame->channel_layout ? frame->channel_layout : av_get_default_channel_layout(frame->channels),
                                         frame->format, frame->sample_rate,
                                         0, NULL);
                if (!swr || swr_init(swr) < 0) {
                    fprintf(stderr, "Failed to initialize resampler\n");
                    goto cleanup_audio;
                }
            }

            // ensure buffer large enough
            int out_nb_samples = av_rescale_rnd(swr_get_delay(swr, frame->sample_rate) + frame->nb_samples, out_rate, frame->sample_rate, AV_ROUND_UP);
            if (out_nb_samples > max_out_samples) {
                if (conv_samples) av_freep(&conv_samples[0]);
                av_freep(&conv_samples);
                int ret = av_samples_alloc_array_and_samples(&conv_samples, &conv_linesize, out_channels,
                                                             out_nb_samples, out_fmt, 0);
                if (ret < 0) {
                    fprintf(stderr, "Could not allocate converted samples\n");
                    goto cleanup_audio;
                }
                max_out_samples = out_nb_samples;
            }

            int converted = swr_convert(swr, conv_samples, out_nb_samples,
                                        (const uint8_t **)frame->data, frame->nb_samples);
            if (converted < 0) {
                fprintf(stderr, "swr_convert error\n");
                goto cleanup_audio;
            }

            int bytes_per_sample = av_get_bytes_per_sample(out_fmt);
            int out_buffer_size = av_samples_get_buffer_size(&conv_linesize, out_channels, converted, out_fmt, 1);
            if (out_buffer_size < 0) {
                fprintf(stderr, "av_samples_get_buffer_size error\n");
                goto cleanup_audio;
            }

            // write to ALSA
            int frames_to_deliver = converted;
            int frames_written = 0;
            uint8_t *data_ptr = conv_samples[0];
            while (frames_to_deliver > 0) {
                snd_pcm_sframes_t frames = snd_pcm_writei(ps->pcm_handle, data_ptr, frames_to_deliver);
                if (frames == -EPIPE) {
                    // underrun
                    snd_pcm_prepare(ps->pcm_handle);
                    continue;
                } else if (frames < 0) {
                    fprintf(stderr, "ALSA write error: %s\n", snd_strerror(frames));
                    break;
                }
                frames_to_deliver -= frames;
                frames_written += frames;
                data_ptr += frames * out_channels * bytes_per_sample;
            }

            // update audio clock: add frames_written / sample_rate
            pthread_mutex_lock(&ps->audio_clock_mutex);
            ps->audio_clock_seconds += (double)frames_written / (double)ps->alsa_samplerate;
            pthread_mutex_unlock(&ps->audio_clock_mutex);
        }
    }

cleanup_audio:
    if (swr) swr_free(&swr);
    if (conv_samples) {
        av_freep(&conv_samples[0]);
        av_freep(&conv_samples);
    }
    av_frame_free(&frame);
    return NULL;
}

/* Helper: get audio clock (seconds) */
static double audio_clock_get(PlayerState *ps) {
    double v;
    pthread_mutex_lock(&ps->audio_clock_mutex);
    v = ps->audio_clock_seconds;
    pthread_mutex_unlock(&ps->audio_clock_mutex);
    return v;
}

static void *video_thread(void *arg)
{
    PlayerState *ps = (PlayerState *)arg;
    AVPacket pkt;
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Failed to allocate frame\n");
        return NULL;
    }

    struct SwsContext *sws = NULL;
    AVFrame *rgb_frame = NULL;
    uint8_t *rgb_buf = NULL;
    int rgb_bufsize = ILI9225_W * ILI9225_H * 2; // full frame

    // Allocate once
    rgb_buf = av_malloc(rgb_bufsize);
    if (!rgb_buf) {
        fprintf(stderr, "Failed to allocate rgb_buf\n");
        av_frame_free(&frame);
        return NULL;
    }

    rgb_frame = av_frame_alloc();
    if (!rgb_frame) {
        fprintf(stderr, "Failed to allocate rgb_frame\n");
        av_free(rgb_buf);
        av_frame_free(&frame);
        return NULL;
    }
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, rgb_buf,
                         AV_PIX_FMT_RGB565LE, ILI9225_W, ILI9225_H, 1);

    while (!ps->quit) {
        int r = packet_queue_get(&ps->videoq, &pkt, 1);
        if (r < 0) break;
        if (r == 0) continue;

        if (avcodec_send_packet(ps->video_dec_ctx, &pkt) < 0) {
            av_packet_unref(&pkt);
            continue;
        }
        av_packet_unref(&pkt);

        while (avcodec_receive_frame(ps->video_dec_ctx, frame) == 0) {

            if (!sws) {
                sws = sws_getContext(frame->width, frame->height, frame->format,
                                     ILI9225_W, ILI9225_H, AV_PIX_FMT_RGB565LE,
                                     SWS_BILINEAR, NULL, NULL, NULL);
                if (!sws) {
                    fprintf(stderr, "Failed to create sws context\n");
                    goto cleanup;
                }
            }

            // Convert to RGB565
            sws_scale(sws, (const uint8_t * const*)frame->data, frame->linesize,
                      0, frame->height, rgb_frame->data, rgb_frame->linesize);

            // Compute PTS and sync with audio
            int64_t pts = (frame->pts != AV_NOPTS_VALUE) ? frame->pts : frame->best_effort_timestamp;
            double pts_seconds = pts * av_q2d(ps->fmt_ctx->streams[ps->video_stream_idx]->time_base);
            double audio_time = audio_clock_get(ps);
            double diff = pts_seconds - audio_time;
            if (diff > 0) {
                usleep((useconds_t)(diff * 1e6));
            } else if (diff < -0.2) {
                continue; // drop late frame
            }

            // Write **entire frame** to ili9225 device
            ssize_t written = 0;
            uint8_t *p = rgb_buf;
            while (written < rgb_bufsize) {
                ssize_t w = write(ps->ili_fd, p + written, rgb_bufsize - written);
                if (w < 0) {
                    if (errno == EINTR) continue;
                    fprintf(stderr, "Write failed: %s\n", strerror(errno));
                    goto cleanup;
                }
                written += w;
            }
        }
    }

cleanup:
    if (sws) sws_freeContext(sws);
    if (rgb_frame) av_frame_free(&rgb_frame);
    if (rgb_buf) av_free(rgb_buf);
    av_frame_free(&frame);
    return NULL;
}


/* Main demux loop: read packets and distribute */
static int demux_loop(PlayerState *ps) {
    AVPacket packet;
    av_init_packet(&packet);

    while (!ps->quit) {
        int ret = av_read_frame(ps->fmt_ctx, &packet);
        if (ret < 0) {
            // EOF or error: push null packets for flushing then break
            // push empty packet with size 0? We'll directly signal EOF by pushing a special packet with data NULL (we can use av_packet_unref)
            break;
        }

        if (packet.stream_index == ps->audio_stream_idx) {
            if (packet_queue_put(&ps->audioq, &packet) < 0) {
                av_packet_unref(&packet);
                break;
            }
        } else if (packet.stream_index == ps->video_stream_idx) {
            if (packet_queue_put(&ps->videoq, &packet) < 0) {
                av_packet_unref(&packet);
                break;
            }
        } else {
            // discard other streams
            av_packet_unref(&packet);
        }
    }

    // EOF: signal queues to stop/flush
    packet_queue_abort(&ps->audioq);
    packet_queue_abort(&ps->videoq);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input-video>\n", argv[0]);
        return 1;
    }
    const char *input = argv[1];

    av_log_set_level(AV_LOG_ERROR);
   // av_register_all();
    avformat_network_init();

    PlayerState state;
    memset(&state, 0, sizeof(state));
    global_state = &state;

    packet_queue_init(&state.audioq, AUDIO_QUEUE_MAX);
    packet_queue_init(&state.videoq, VIDEO_QUEUE_MAX);
    pthread_mutex_init(&state.audio_clock_mutex, NULL);
    state.audio_clock_seconds = 0.0;

    // Open input
    if (avformat_open_input(&state.fmt_ctx, input, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open input %s\n", input);
        return 1;
    }
    if (avformat_find_stream_info(state.fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Failed to find stream info\n");
        return 1;
    }

    // find audio & video streams
    state.audio_stream_idx = av_find_best_stream(state.fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    state.video_stream_idx = av_find_best_stream(state.fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (state.audio_stream_idx < 0 && state.video_stream_idx < 0) {
        fprintf(stderr, "No audio or video stream found\n");
        return 1;
    }

    // open decoders
    if (state.audio_stream_idx >= 0) {
        AVStream *as = state.fmt_ctx->streams[state.audio_stream_idx];
        AVCodec *adec = avcodec_find_decoder(as->codecpar->codec_id);
        if (!adec) { fprintf(stderr, "Unsupported audio codec\n"); return 1; }
        state.audio_dec_ctx = avcodec_alloc_context3(adec);
        avcodec_parameters_to_context(state.audio_dec_ctx, as->codecpar);
        if (avcodec_open2(state.audio_dec_ctx, adec, NULL) < 0) {
            fprintf(stderr, "Failed to open audio codec\n"); return 1;
        }
    }

    if (state.video_stream_idx >= 0) {
        AVStream *vs = state.fmt_ctx->streams[state.video_stream_idx];
        AVCodec *vdec = avcodec_find_decoder(vs->codecpar->codec_id);
        if (!vdec) { fprintf(stderr, "Unsupported video codec\n"); return 1; }
        state.video_dec_ctx = avcodec_alloc_context3(vdec);
        avcodec_parameters_to_context(state.video_dec_ctx, vs->codecpar);
        if (avcodec_open2(state.video_dec_ctx, vdec, NULL) < 0) {
            fprintf(stderr, "Failed to open video codec\n"); return 1;
        }
    }

    // Setup ALSA (audio)
    if (state.audio_stream_idx >= 0) {
        // choose ALSA parameters
        state.alsa_samplerate = state.audio_dec_ctx->sample_rate;
        if (state.alsa_samplerate <= 0) state.alsa_samplerate = 44100;
        state.alsa_channels = state.audio_dec_ctx->channels;
        if (state.alsa_channels <= 0) state.alsa_channels = 2;
        state.alsa_format = SND_PCM_FORMAT_S16_LE;

        int err = snd_pcm_open(&state.pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
        if (err < 0) {
            fprintf(stderr, "Cannot open ALSA device: %s\n", snd_strerror(err));
            return 1;
        }
        err = snd_pcm_set_params(state.pcm_handle,
                                 SND_PCM_FORMAT_S16_LE,
                                 SND_PCM_ACCESS_RW_INTERLEAVED,
                                 state.alsa_channels,
                                 state.alsa_samplerate,
                                 1, // allow resample
                                 500000); // 0.5sec latency
        if (err < 0) {
            fprintf(stderr, "Cannot set ALSA params: %s\n", snd_strerror(err));
            return 1;
        }
    }

    // open ILI9225 device
    state.ili_fd = open(ILI9225_DEV, O_WRONLY);
    if (state.ili_fd < 0) {
        fprintf(stderr, "Warning: cannot open %s: %s\nVideo output will fail.\n", ILI9225_DEV, strerror(errno));
        // We'll keep going; video writes will likely fail unless device exists
    }

    // start threads
    if (state.audio_stream_idx >= 0) {
        pthread_create(&state.audio_tid, NULL, audio_thread, &state);
    }
    if (state.video_stream_idx >= 0) {
        pthread_create(&state.video_tid, NULL, video_thread, &state);
    }

    // demux loop
    demux_loop(&state);

    // wait threads to finish (we aborted queues on EOF)
    if (state.audio_stream_idx >= 0) {
        packet_queue_abort(&state.audioq);
        pthread_join(state.audio_tid, NULL);
    }
    if (state.video_stream_idx >= 0) {
        packet_queue_abort(&state.videoq);
        pthread_join(state.video_tid, NULL);
    }

    // cleanup
    if (state.ili_fd >= 0) close(state.ili_fd);
    if (state.pcm_handle) {
        snd_pcm_drain(state.pcm_handle);
        snd_pcm_close(state.pcm_handle);
    }
    if (state.audio_dec_ctx) avcodec_free_context(&state.audio_dec_ctx);
    if (state.video_dec_ctx) avcodec_free_context(&state.video_dec_ctx);
    if (state.fmt_ctx) avformat_close_input(&state.fmt_ctx);

    packet_queue_destroy(&state.audioq);
    packet_queue_destroy(&state.videoq);
    pthread_mutex_destroy(&state.audio_clock_mutex);

    avformat_network_deinit();
    return 0;
}

