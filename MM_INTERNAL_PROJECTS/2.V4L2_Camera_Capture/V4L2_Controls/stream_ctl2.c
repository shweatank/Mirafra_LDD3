#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>

#define VIDEO_DEVICE "/dev/video0"
#define CTRL_DEVICE  "/dev/v4l-subdev0"
#define WIDTH        640
#define HEIGHT       480
#define NUM_BUFFERS  4

struct buffer {
    void   *start;
    size_t  length;
};

struct buffer *buffers;

// Clamp function
static inline uint8_t clamp(int val) {
    if (val < 0) return 0;
    if (val > 255) return 255;
    return (uint8_t)val;
}

// Set subdev control
int set_control(int fd, int id, int value) {
    struct v4l2_control ctrl;
    ctrl.id = id;
    ctrl.value = value;
    if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
        perror("VIDIOC_S_CTRL");
        return -1;
    }
    return 0;
}

int main() {
    int fd = open(VIDEO_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Cannot open video device");
        return 1;
    }

    int fd_ctrl = open(CTRL_DEVICE, O_RDWR);
    if (fd_ctrl < 0) {
        perror("Cannot open control device");
        close(fd);
        return 1;
    }

    // Set GREY8 format
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = WIDTH;
    fmt.fmt.pix.height      = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
    fmt.fmt.pix.field       = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        close(fd);
        close(fd_ctrl);
        return 1;
    }

    // Request buffers
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count  = NUM_BUFFERS;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        close(fd);
        close(fd_ctrl);
        return 1;
    }

    buffers = calloc(req.count, sizeof(*buffers));
    for (int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            close(fd);
            close(fd_ctrl);
            return 1;
        }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length,
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            perror("mmap");
            close(fd);
            close(fd_ctrl);
            return 1;
        }
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            close(fd);
            close(fd_ctrl);
            return 1;
        }
    }

    // Start streaming
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        close(fd);
        close(fd_ctrl);
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        close(fd);
        close(fd_ctrl);
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("OV7251 Live - GREY8",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGB24,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             WIDTH, HEIGHT);

    uint8_t *rgb_frame = malloc(WIDTH * HEIGHT * 3);
    if (!rgb_frame) {
        fprintf(stderr, "Failed to allocate RGB frame\n");
        close(fd);
        close(fd_ctrl);
        return 1;
    }

    // Initial control values
    int exposure = 400; // default from your v4l2-ctl
    int gain     = 16;  
    int hflip    = 0;
    int vflip    = 0;

    printf("Controls:\n");
    printf("  Arrow Up/Down    : Exposure +/-\n");
    printf("  Arrow Left/Right : Gain -/+\n");
    printf("  h                : Toggle Horizontal Flip\n");
    printf("  v                : Toggle Vertical Flip\n");
    printf("  ESC / Close      : Exit\n");

    int running = 1;
    while (running) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("VIDIOC_DQBUF");
            break;
        }

        uint8_t *grey = buffers[buf.index].start;

        // Copy grey -> RGB for SDL
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            rgb_frame[i*3 + 0] = grey[i];
            rgb_frame[i*3 + 1] = grey[i];
            rgb_frame[i*3 + 2] = grey[i];
        }

        SDL_UpdateTexture(texture, NULL, rgb_frame, WIDTH*3);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            break;
        }

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: running = 0; break;
                    case SDLK_UP:    exposure += 10; set_control(fd_ctrl, V4L2_CID_EXPOSURE, exposure); break;
                    case SDLK_DOWN:  exposure -= 10; set_control(fd_ctrl, V4L2_CID_EXPOSURE, exposure); break;
                    case SDLK_RIGHT: gain += 1;      set_control(fd_ctrl, V4L2_CID_GAIN, gain); break;
                    case SDLK_LEFT:  gain -= 1;      set_control(fd_ctrl, V4L2_CID_GAIN, gain); break;
                    case SDLK_h:     hflip = !hflip; set_control(fd_ctrl, V4L2_CID_HFLIP, hflip); printf("HFlip=%d\n", hflip); break;
                    case SDLK_v:     vflip = !vflip; set_control(fd_ctrl, V4L2_CID_VFLIP, vflip); printf("VFlip=%d\n", vflip); break;
                }
            }
        }
    }

    free(rgb_frame);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (int i = 0; i < req.count; i++)
        munmap(buffers[i].start, buffers[i].length);

    free(buffers);
    close(fd);
    close(fd_ctrl);
    return 0;
}

