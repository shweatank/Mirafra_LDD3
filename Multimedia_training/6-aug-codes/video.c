#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define DEVICE "/dev/video0"
#define WIDTH 640
#define HEIGHT 480
#define NUM_BUFFERS 4
#define NUM_FRAMES 100

typedef struct {
    void *start;
    size_t length;
} Buffer;

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open video device");
        return 1;
    }

    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("Failed to query capabilities");
        return 1;
    }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("Failed to set format");
        return 1;
    }

    struct v4l2_requestbuffers req = {0};
    req.count = NUM_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Failed to request buffers");
        return 1;
    }

    Buffer *buffers = calloc(req.count, sizeof(Buffer));
    for (int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("Failed to query buffer");
            return 1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            perror("Failed to mmap");
            return 1;
        }

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Failed to queue buffer");
            return 1;
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("Failed to start streaming");
        return 1;
    }

    FILE *output = fopen("video.raw", "wb");
    if (!output) {
        perror("Failed to open output file");
        return 1;
    }

    for (int i = 0; i < NUM_FRAMES; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("Failed to dequeue buffer");
            break;
        }

        fwrite(buffers[buf.index].start, 1, buf.bytesused, output);

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Failed to requeue buffer");
            break;
        }
    }

    fclose(output);
    for (int i = 0; i < req.count; ++i) {
        munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers);

    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("Failed to stop streaming");
        return 1;
    }

    close(fd);
    printf("Captured %d frames to video.raw\n", NUM_FRAMES);
    return 0;
}

