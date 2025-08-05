// v4l2_capture_video.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480
#define FRAME_COUNT 150  // ~3 seconds at 30 FPS

int main() {
    int fd = open("/dev/video0", O_RDWR);
    if (fd == -1) {
        perror("Opening video device");
        return 1;
    }

    // Set video format
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("Setting Pixel Format");
        return 1;
    }

    // Request buffer
    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Requesting Buffer");
        return 1;
    }

    // Map buffers
    void *buffers[4];
    struct v4l2_buffer buf = {0};

    for (int i = 0; i < 4; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("Querying Buffer");
            return 1;
        }
        buffers[i] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i] == MAP_FAILED) {
            perror("Memory Mapping Failed");
            return 1;
        }
    }

    // Queue buffers
    for (int i = 0; i < 4; i++) {
        buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Queue Buffer");
            return 1;
        }
    }

    // Start streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("Stream On");
        return 1;
    }

    FILE *out = fopen("video.yuv", "wb");
    if (!out) {
        perror("File open");
        return 1;
    }

    printf("Recording video...\n");

    for (int i = 0; i < FRAME_COUNT; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("Dequeue Buffer");
            continue;
        }

        fwrite(buffers[buf.index], buf.bytesused, 1, out);

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Requeue Buffer");
            return 1;
        }
    }

    printf("Done recording.\n");

    fclose(out);

    // Stop streaming
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("Stream Off");
        return 1;
    }

    for (int i = 0; i < 4; i++) {
        munmap(buffers[i], buf.length);
    }

    close(fd);
    return 0;
}
