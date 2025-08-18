// Corrected C code for synchronous audio-video capture, playback, and saving
// Dependencies: ALSA, V4L2, SDL2

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 480
#define AUDIO_BUFFER_FRAMES 1024
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS 2

struct buffer {
    void *start;
    size_t length;
};

int video_fd;
struct buffer *buffers;
unsigned int n_buffers;

snd_pcm_t *audio_handle;
FILE *vfile, *afile;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

volatile int running = 1;

void* video_thread(void *arg) {
    struct v4l2_buffer buf;
    while (running) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(video_fd, VIDIOC_DQBUF, &buf) == -1) {
            perror("VIDIOC_DQBUF");
            continue;
        }

        if (buf.index >= n_buffers) {
            fprintf(stderr, "Invalid buffer index\n");
            continue;
        }

        fwrite(buffers[buf.index].start, 1, buf.bytesused, vfile);

        if (SDL_UpdateTexture(texture, NULL, buffers[buf.index].start, WIDTH * 2) < 0) {
            fprintf(stderr, "SDL_UpdateTexture error: %s\n", SDL_GetError());
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        if (ioctl(video_fd, VIDIOC_QBUF, &buf) == -1) {
            perror("VIDIOC_QBUF");
            continue;
        }
    }
    return NULL;
}

void* audio_thread(void *arg) {
    int16_t buffer[AUDIO_BUFFER_FRAMES * AUDIO_CHANNELS];
    while (running) {
        int frames = snd_pcm_readi(audio_handle, buffer, AUDIO_BUFFER_FRAMES);
        if (frames < 0) {
            snd_pcm_prepare(audio_handle);
            continue;
        }
        fwrite(buffer, sizeof(int16_t) * AUDIO_CHANNELS, frames, afile);
    }
    return NULL;
}

void init_video() {
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;

    video_fd = open("/dev/video0", O_RDWR);
    if (video_fd < 0) {
        perror("Cannot open video device");
        exit(1);
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    if (ioctl(video_fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("VIDIOC_S_FMT");
        exit(1);
    }

    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(video_fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("VIDIOC_REQBUFS");
        exit(1);
    }

    buffers = calloc(req.count, sizeof(*buffers));
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        if (ioctl(video_fd, VIDIOC_QUERYBUF, &buf) == -1) {
            perror("VIDIOC_QUERYBUF");
            exit(1);
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                                        MAP_SHARED, video_fd, buf.m.offset);
        if (buffers[n_buffers].start == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }
    }

    for (unsigned int i = 0; i < n_buffers; ++i) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(video_fd, VIDIOC_QBUF, &buf) == -1) {
            perror("VIDIOC_QBUF");
            exit(1);
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(video_fd, VIDIOC_STREAMON, &type) == -1) {
        perror("VIDIOC_STREAMON");
        exit(1);
    }
}

void init_audio() {
    snd_pcm_hw_params_t *params;
    snd_pcm_open(&audio_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(audio_handle, params);
    snd_pcm_hw_params_set_access(audio_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(audio_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(audio_handle, params, AUDIO_CHANNELS);
    unsigned int rate = AUDIO_SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(audio_handle, params, &rate, 0);
    snd_pcm_hw_params_set_period_size_near(audio_handle, params, (snd_pcm_uframes_t[]){AUDIO_BUFFER_FRAMES}, 0);
    snd_pcm_hw_params(audio_handle, params);
}

void init_sdl() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Video", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YUY2,
                                SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    if (!window || !renderer || !texture) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        exit(1);
    }
}

int main() {
    vfile = fopen("video.yuv", "wb");
    afile = fopen("audio.wav", "wb+");

    uint8_t header[44] = {0};
    fwrite(header, 1, 44, afile);

    init_video();
    init_audio();
    init_sdl();

    pthread_t vtid, atid;
    pthread_create(&vtid, NULL, video_thread, NULL);
    pthread_create(&atid, NULL, audio_thread, NULL);

    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_Delay(10);
    }

    pthread_join(vtid, NULL);
    pthread_join(atid, NULL);

    fseek(afile, 0, SEEK_SET);
    uint32_t data_chunk_size = ftell(afile) - 44;

    memcpy(header, "RIFF", 4);
    *(uint32_t *)(header + 4) = 36 + data_chunk_size;
    memcpy(header + 8, "WAVEfmt ", 8);
    *(uint32_t *)(header + 16) = 16;
    *(uint16_t *)(header + 20) = 1;
    *(uint16_t *)(header + 22) = AUDIO_CHANNELS;
    *(uint32_t *)(header + 24) = AUDIO_SAMPLE_RATE;
    *(uint32_t *)(header + 28) = AUDIO_SAMPLE_RATE * AUDIO_CHANNELS * 2;
    *(uint16_t *)(header + 32) = AUDIO_CHANNELS * 2;
    *(uint16_t *)(header + 34) = 16;
    memcpy(header + 36, "data", 4);
    *(uint32_t *)(header + 40) = data_chunk_size;

    fwrite(header, 1, 44, afile);
    fclose(vfile);
    fclose(afile);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    snd_pcm_close(audio_handle);
    close(video_fd);
    return 0;
}
