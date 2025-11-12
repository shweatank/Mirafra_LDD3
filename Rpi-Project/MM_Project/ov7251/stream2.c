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
#define PIX_FMT V4L2_PIX_FMT_GREY
#define NUM_BUFFERS 4

struct buffer {
    void *start;
    size_t length;
};

int main() {
    int fd;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;
    struct buffer buffers[NUM_BUFFERS];
    unsigned int i;

    // 1. Open device
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Opening video device");
        return 1;
    }

    // 2. Set format
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = PIX_FMT;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("Setting Pixel Format");
        return 1;
    }

    printf("Using resolution %dx%d, pixelformat GREY\n", fmt.fmt.pix.width, fmt.fmt.pix.height);

    // 3. Request buffers
    memset(&req, 0, sizeof(req));
    req.count = NUM_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Requesting Buffers");
        return 1;
    }

    // 4. Map buffers
    for (i = 0; i < NUM_BUFFERS; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("Querying Buffer");
            return 1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if (buffers[i].start == MAP_FAILED) {
            perror("Mapping Buffer");
            return 1;
        }
    }

    // 5. Queue buffers
    for (i = 0; i < NUM_BUFFERS; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Queue Buffer");
            return 1;
        }
    }

    // 6. Start streaming
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("Start Capture");
        return 1;
    }

    printf("Capturing 100 frames...\n");

    // 7. Capture loop
    for (i = 0; i < 100; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        // Dequeue buffer
        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("Dequeue Buffer");
            return 1;
        }

        printf("Captured frame %d\n", i + 1);

        // Optional: save to file
        char filename[64];
        snprintf(filename, sizeof(filename), "frame_%03d.raw", i + 1);
        FILE *f = fopen(filename, "wb");
        if (f) {
            fwrite(buffers[buf.index].start, 1, buf.bytesused, f);
            fclose(f);
        }

        // Requeue buffer
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Requeue Buffer");
            return 1;
        }
    }

    // 8. Stop streaming
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("Stop Capture");
        return 1;
    }

    // 9. Unmap buffers
    for (i = 0; i < NUM_BUFFERS; i++)
        munmap(buffers[i].start, buffers[i].length);

    close(fd);
    printf("Capture complete.\n");
    return 0;
}

