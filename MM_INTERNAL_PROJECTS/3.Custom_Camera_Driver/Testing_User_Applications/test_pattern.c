#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>

#define DEVICE "/dev/video0"
#define SUBDEV "/dev/v4l-subdev0"
#define WIDTH  640
#define HEIGHT 480
#define BUFFER_COUNT 4

struct buffer {
    void *start;
    size_t length;
};

static int fd = -1;
static int subdev_fd = -1;
static struct buffer *buffers = NULL;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static int test_pattern_enabled = 0;

static void cleanup() {
    if (buffers) {
        for (int i = 0; i < BUFFER_COUNT; i++) {
            if (buffers[i].start)
                munmap(buffers[i].start, buffers[i].length);
        }
        free(buffers);
    }
    if (fd >= 0)
        close(fd);
    if (subdev_fd >= 0)
        close(subdev_fd);
    if (texture)
        SDL_DestroyTexture(texture);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

static void toggle_test_pattern() {
    struct v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_TEST_PATTERN;

    test_pattern_enabled = !test_pattern_enabled;
    ctrl.value = test_pattern_enabled;

    if (ioctl(subdev_fd, VIDIOC_S_CTRL, &ctrl) < 0) {
        perror("VIDIOC_S_CTRL (test pattern)");
    } else {
        printf("Test pattern %s\n", test_pattern_enabled ? "ENABLED" : "DISABLED");
    }
}

int main() {
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;
    SDL_Event e;

    // --- Open devices ---
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Cannot open video device");
        return 1;
    }

    subdev_fd = open(SUBDEV, O_RDWR);
    if (subdev_fd < 0) {
        perror("Cannot open subdevice (for test pattern)");
        close(fd);
        return 1;
    }

    // --- Set video format (8-bit grayscale) ---
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        cleanup();
        return 1;
    }

    // --- Request buffers ---
    memset(&req, 0, sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        cleanup();
        return 1;
    }

    // --- Map buffers ---
    buffers = calloc(req.count, sizeof(*buffers));
    for (int i = 0; i < req.count; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = req.type;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            cleanup();
            return 1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if (buffers[i].start == MAP_FAILED) {
            perror("mmap");
            cleanup();
            return 1;
        }

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            cleanup();
            return 1;
        }
    }

    // --- Start streaming ---
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        cleanup();
        return 1;
    }

    // --- Initialize SDL ---
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        cleanup();
        return 1;
    }

    window = SDL_CreateWindow("OV7251 Viewer (8-bit)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    printf("Streaming started. Press 'T' to toggle test pattern, 'ESC' to quit.\n");

    // --- Main loop ---
    while (1) {
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                break;
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    break;
                else if (e.key.keysym.sym == SDLK_t)
                    toggle_test_pattern();
            }
        }

        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            if (errno == EAGAIN)
                continue;
            perror("VIDIOC_DQBUF");
            break;
        }

        // --- Convert GREY -> RGB24 ---
        unsigned char *grey = buffers[buf.index].start;
        unsigned char *rgb = malloc(WIDTH * HEIGHT * 3);
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            rgb[i * 3 + 0] = grey[i];
            rgb[i * 3 + 1] = grey[i];
            rgb[i * 3 + 2] = grey[i];
        }

        SDL_UpdateTexture(texture, NULL, rgb, WIDTH * 3);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        free(rgb);

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            break;
        }
    }

    printf("Stopping stream...\n");

    // --- Stop streaming ---
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
        perror("VIDIOC_STREAMOFF");

    cleanup();
    return 0;
}

