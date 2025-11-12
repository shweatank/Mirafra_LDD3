#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <signal.h>
#include <SDL2/SDL.h>

#define VIDEO_DEVICE "/dev/video0"
#define SUBDEV_DEVICE "/dev/v4l-subdev0"
#define WIDTH 640
#define HEIGHT 480
#define BUFFER_COUNT 4

struct buffer {
    void *start;
    size_t length;
};

static int fd_video = -1;
static int fd_subdev = -1;
static struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
static volatile int keep_running = 1;

void int_handler(int dummy) {
    (void)dummy;
    keep_running = 0;
}

int xioctl(int fh, int request, void *arg) {
    int r;
    do {
        r = ioctl(fh, request, arg);
    } while (r == -1 && errno == EINTR);
    return r;
}

// Set subdevice control safely
int set_subdev_control(int fd, __u32 id, int value) {
    struct v4l2_control ctrl;
    ctrl.id = id;
    ctrl.value = value;
    if (xioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
        fprintf(stderr, "Failed to set subdev control 0x%08x -> %s\n", id, strerror(errno));
        return -1;
    }
    printf("Set subdev control 0x%08x = %d\n", id, value);
    return 0;
}

// Initialize the subdevice (sensor controls)
int init_subdev() {
    fd_subdev = open(SUBDEV_DEVICE, O_RDWR);
    if (fd_subdev < 0) {
        perror("Opening subdevice");
        return -1;
    }

    // Example: set exposure, horizontal and vertical flip
    set_subdev_control(fd_subdev, V4L2_CID_EXPOSURE, 300); // Adjust as needed
    set_subdev_control(fd_subdev, V4L2_CID_HFLIP, 0);
    set_subdev_control(fd_subdev, V4L2_CID_VFLIP, 0);

    set_subdev_control(fd_subdev, 0x009e0903, 0);

    return 0;
}

// Initialize video capture device
int init_video() {
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;

    fd_video = open(VIDEO_DEVICE, O_RDWR);
    if (fd_video < 0) {
        perror("Opening video device");
        return -1;
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY; // 10-bit grayscale
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (xioctl(fd_video, VIDIOC_S_FMT, &fmt) < 0) {
        perror("Setting pixel format");
        return -1;
    }

    memset(&req, 0, sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (xioctl(fd_video, VIDIOC_REQBUFS, &req) < 0) {
        perror("Requesting buffers");
        return -1;
    }

    buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        perror("Allocating buffers");
        return -1;
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if (xioctl(fd_video, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("Querying buffer");
            return -1;
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_video, buf.m.offset);

        if (buffers[n_buffers].start == MAP_FAILED) {
            perror("Mapping buffer");
            return -1;
        }
    }

    return 0;
}

int start_capturing() {
    for (unsigned int i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd_video, VIDIOC_QBUF, &buf) < 0) {
            perror("Queue buffer");
            return -1;
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd_video, VIDIOC_STREAMON, &type) < 0) {
        perror("Start capture");
        return -1;
    }
    return 0;
}

// Convert Y10 to RGB24
void y10_to_rgb24_scaled(uint16_t *src, uint8_t *dst, int size) {
    for (int i = 0; i < size; ++i) {
        uint16_t val10 = src[i] & 0x03FF;
        uint8_t scaled = (uint8_t)((val10 * 255) / 1023);
        dst[3*i + 0] = scaled;
        dst[3*i + 1] = scaled;
        dst[3*i + 2] = scaled;
    }
}

int main() {
    signal(SIGINT, int_handler);

    if (init_subdev() < 0) return 1;   // Configure sensor controls
    if (init_video() < 0) return 1;    // Initialize capture device
    if (start_capturing() < 0) return 1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Y10 Video",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT,
                                          SDL_WINDOW_MINIMIZED);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    uint8_t *frame_rgb = malloc(WIDTH * HEIGHT * 3);

    printf("Streaming Y10 video... Press Ctrl+C to stop.\n");

    while (keep_running) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(fd_video, &fds);
        tv.tv_sec = 2; tv.tv_usec = 0;

        r = select(fd_video + 1, &fds, NULL, NULL, &tv);
        if (r <= 0) continue;

        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (xioctl(fd_video, VIDIOC_DQBUF, &buf) < 0) continue;

        y10_to_rgb24_scaled((uint16_t *)buffers[buf.index].start, frame_rgb, WIDTH * HEIGHT);

        SDL_UpdateTexture(texture, NULL, frame_rgb, WIDTH * 3);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        xioctl(fd_video, VIDIOC_QBUF, &buf);
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd_video, VIDIOC_STREAMOFF, &type);

    for (unsigned int i = 0; i < n_buffers; ++i)
        munmap(buffers[i].start, buffers[i].length);
    free(buffers);
    free(frame_rgb);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    if (fd_video >= 0) close(fd_video);
    if (fd_subdev >= 0) close(fd_subdev);

    printf("Streaming stopped. Clean exit.\n");
    return 0;
}

