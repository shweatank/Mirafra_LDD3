/*
 * Complete C code to capture, record, and play audio + video in sync.
 * Video: V4L2 + SDL2
 * Audio: ALSA
 * Output: video.raw + audio.raw
 *
 * Build with:
 * gcc av_sync_capture.c -o av_sync_capture -lpthread -lSDL2 -lasound
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>
#include <alsa/asoundlib.h>

#define VIDEO_DEVICE "/dev/video0"
#define AUDIO_DEVICE "default"

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define FRAME_RATE 30
#define AUDIO_RATE 44100
#define AUDIO_CHANNELS 2
#define AUDIO_FORMAT SND_PCM_FORMAT_S16_LE
#define AUDIO_BUFFER_FRAMES 1024

volatile int running = 1;

void handle_sigint(int sig) {
    running = 0;
}

typedef struct {
    void *start;
    size_t length;
} Buffer;

Buffer *video_buffers;
int video_fd;
uint8_t *video_frame_buffer;
FILE *video_out;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

snd_pcm_t *capture_handle;
snd_pcm_t *playback_handle;
FILE *audio_out;

void *video_thread(void *arg) {
    struct v4l2_buffer buf;
    SDL_Event event;

    while (running) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(video_fd, VIDIOC_DQBUF, &buf) == -1) {
            perror("VIDIOC_DQBUF");
            break;
        }

        memcpy(video_frame_buffer, video_buffers[buf.index].start, buf.bytesused);
        fwrite(video_frame_buffer, 1, buf.bytesused, video_out);

        SDL_UpdateTexture(texture, NULL, video_frame_buffer, VIDEO_WIDTH * 2);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        if (ioctl(video_fd, VIDIOC_QBUF, &buf) == -1) {
            perror("VIDIOC_QBUF");
            break;
        }

        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            running = 0;
        }

        usleep(1000000 / FRAME_RATE);
    }

    return NULL;
}

void *audio_thread(void *arg) {
    int16_t buffer[AUDIO_BUFFER_FRAMES * AUDIO_CHANNELS];

    while (running) {
        snd_pcm_readi(capture_handle, buffer, AUDIO_BUFFER_FRAMES);
        snd_pcm_writei(playback_handle, buffer, AUDIO_BUFFER_FRAMES);
        fwrite(buffer, sizeof(int16_t), AUDIO_BUFFER_FRAMES * AUDIO_CHANNELS, audio_out);
    }

    return NULL;
}

int init_video() {
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;

    video_fd = open(VIDEO_DEVICE, O_RDWR);
    if (video_fd == -1) {
        perror("Opening video device");
        return -1;
    }

    if (ioctl(video_fd, VIDIOC_QUERYCAP, &cap) == -1) {
        perror("Querying Capabilities");
        return -1;
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = VIDEO_WIDTH;
    fmt.fmt.pix.height = VIDEO_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(video_fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("Setting Pixel Format");
        return -1;
    }

    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(video_fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("Requesting Buffer");
        return -1;
    }

    video_buffers = calloc(req.count, sizeof(Buffer));
    for (int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        ioctl(video_fd, VIDIOC_QUERYBUF, &buf);

        video_buffers[i].length = buf.length;
        video_buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, video_fd, buf.m.offset);

        ioctl(video_fd, VIDIOC_QBUF, &buf);
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(video_fd, VIDIOC_STREAMON, &type);

    video_frame_buffer = malloc(VIDEO_WIDTH * VIDEO_HEIGHT * 2);
    video_out = fopen("video.raw", "wb");

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Video", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, VIDEO_WIDTH, VIDEO_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YUY2, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH, VIDEO_HEIGHT);

    return 0;
}

int init_audio() {
    snd_pcm_hw_params_t *hw_params;

    snd_pcm_open(&capture_handle, AUDIO_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, AUDIO_FORMAT);
    snd_pcm_hw_params_set_rate(capture_handle, hw_params, AUDIO_RATE, 0);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, AUDIO_CHANNELS);
    snd_pcm_hw_params(capture_handle, hw_params);
    snd_pcm_prepare(capture_handle);
    snd_pcm_hw_params_free(hw_params);

    snd_pcm_open(&playback_handle, AUDIO_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(playback_handle, hw_params);
    snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playback_handle, hw_params, AUDIO_FORMAT);
    snd_pcm_hw_params_set_rate(playback_handle, hw_params, AUDIO_RATE, 0);
    snd_pcm_hw_params_set_channels(playback_handle, hw_params, AUDIO_CHANNELS);
    snd_pcm_hw_params(playback_handle, hw_params);
    snd_pcm_prepare(playback_handle);
    snd_pcm_hw_params_free(hw_params);

    audio_out = fopen("audio.raw", "wb");

    return 0;
}

int main() {
    signal(SIGINT, handle_sigint);

    if (init_video() < 0 || init_audio() < 0) {
        fprintf(stderr, "Initialization failed\n");
        return 1;
    }

    pthread_t vthread, athread;
    pthread_create(&vthread, NULL, video_thread, NULL);
    pthread_create(&athread, NULL, audio_thread, NULL);

    pthread_join(vthread, NULL);
    pthread_join(athread, NULL);

    fclose(video_out);
    fclose(audio_out);
    snd_pcm_close(capture_handle);
    snd_pcm_close(playback_handle);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    close(video_fd);

    return 0;
}
