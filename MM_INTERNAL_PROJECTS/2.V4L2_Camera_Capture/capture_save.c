/*
 * capture_ov7251.c
 * Capture grayscale frames from OV7251 (V4L2_PIX_FMT_GREY)
 *
 * Compile:
 *   gcc capture_ov7251.c -o capture_ov7251
 *
 * Usage:
 *   sudo ./capture_ov7251 /dev/video0 640 480 10
 *   (captures 10 grayscale frames as frame-001.pgm etc.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

struct buffer {
    void   *start;
    size_t  length;
};

static int xioctl(int fd, int request, void *arg)
{
    int r;
    do r = ioctl(fd, request, arg);
    while (r == -1 && errno == EINTR);
    return r;
}

/* Save grayscale frame as PGM */
static int save_pgm(const char *filename, unsigned char *data, int width, int height)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }
    fprintf(fp, "P5\n%d %d\n255\n", width, height);
    fwrite(data, 1, width * height, fp);
    fclose(fp);
    return 0;
}

int main(int argc, char **argv)
{
    const char *devname = "/dev/video0";
    int width = 640, height = 480, frame_count = 10;

    if (argc >= 2) devname = argv[1];
    if (argc >= 4) {
        width = atoi(argv[2]);
        height = atoi(argv[3]);
    }
    if (argc >= 5) frame_count = atoi(argv[4]);

    int fd = open(devname, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) {
        perror("Opening video device");
        return 1;
    }

    struct v4l2_capability cap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP");
        return 1;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "Device does not support capture\n");
        return 1;
    }
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "Device does not support streaming\n");
        return 1;
    }

    /* Set format: GREY */
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return 1;
    }

    printf("Format set: %dx%d, pixel format GREY (%.4s)\n",
           fmt.fmt.pix.width, fmt.fmt.pix.height,
           (char *)&fmt.fmt.pix.pixelformat);

    /* Request buffers */
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        return 1;
    }

    struct buffer *buffers = calloc(req.count, sizeof(*buffers));
    for (size_t i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            return 1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length,
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            perror("mmap");
            return 1;
        }
    }

    /* Queue buffers */
    for (size_t i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            return 1;
        }
    }

    /* Start streaming */
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        return 1;
    }

    printf("Capturing %d grayscale frames...\n", frame_count);

    for (int i = 0; i < frame_count; i++) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        struct timeval tv = {2, 0};
        int r = select(fd + 1, &fds, NULL, NULL, &tv);
        if (r == -1) {
            if (EINTR == errno) continue;
            perror("select");
            break;
        }
        if (r == 0) {
            fprintf(stderr, "Timeout waiting for frame\n");
            break;
        }

        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("VIDIOC_DQBUF");
            break;
        }

        char filename[64];
        snprintf(filename, sizeof(filename), "frame-%03d.pgm", i);
        save_pgm(filename, buffers[buf.index].start, fmt.fmt.pix.width, fmt.fmt.pix.height);
        printf("Saved %s (%u bytes)\n", filename, buf.bytesused);

        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            break;
        }
    }

    /* Stop streaming */
    if (xioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
        perror("VIDIOC_STREAMOFF");

    for (size_t i = 0; i < req.count; i++)
        munmap(buffers[i].start, buffers[i].length);
    free(buffers);
    close(fd);

    printf("Done.\n");
    return 0;
}
