/*
 v4l2_sender.c
 Compile: gcc -O2 -o v4l2_sender v4l2_sender.c
 Usage: ./v4l2_sender <pi_ip> <port> /dev/videoX
 Sends repeated frames of 128x64 grayscale (8192 bytes) with a 4-byte big-endian length header.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/videodev2.h>
#include <time.h>

#define OUT_W 128
#define OUT_H 32
#define FRAME_BYTES (OUT_W * OUT_H)

struct buffer { void *start; size_t length; };

static inline uint8_t yuv_to_gray_pixel(int y, int u, int v) {
    // approximate grayscale from Y (use Y directly, clamp)
    if (y < 0) y = 0;
    if (y > 255) y = 255;
    return (uint8_t)y;
}

// Convert YUYV buffer (src_w x src_h) to RGB not needed — we extract Y and resize to OUT_WxOUT_H
void resize_yuyv_to_gray(const uint8_t *yuyv, int src_w, int src_h, uint8_t *dst_gray) {
    // nearest neighbor on Y channel
    for (int y = 0; y < OUT_H; y++) {
        int sy = y * src_h / OUT_H;
        for (int x = 0; x < OUT_W; x++) {
            int sx = x * src_w / OUT_W;
            // For YUYV: each macropixel has 2 pixels -> index = (sy*src_w + sx) * 2
            int idx = (sy * src_w + sx) * 2;
            int Y = yuyv[idx]; // Y for that pixel
            dst_gray[y*OUT_W + x] = yuv_to_gray_pixel(Y, 0, 0);
        }
    }
}

int set_fmt_yuyv(int fd, int w, int h) {
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = w;
    fmt.fmt.pix.height = h;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return -1;
    }
    // report what we actually got
    fprintf(stderr, "Set format: %u x %u, pixfmt=0x%08x, bytes=%u\n",
            fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.pixelformat, fmt.fmt.pix.sizeimage);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "Usage: %s <pi_ip> <port> /dev/videoX\n", argv[0]); return 1; }
    const char *pi_ip = argv[1];
    int port = atoi(argv[2]);
    const char *devname = argv[3];

    int fd = open(devname, O_RDWR);
    if (fd < 0) { perror("open video"); return 1; }

    // set YUYV 320x240
    int src_w = 320, src_h = 240;
    if (set_fmt_yuyv(fd, src_w, src_h) < 0) { close(fd); return 1; }

    // request buffers (mmap)
    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); close(fd); return 1; }
    if (req.count < 1) { fprintf(stderr, "No buffers available\n"); close(fd); return 1; }

    struct buffer *buffers = calloc(req.count, sizeof(struct buffer));
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); close(fd); return 1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) { perror("mmap"); close(fd); return 1; }
    }

    // queue all buffers
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); close(fd); return 1; }
    }

    // start streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); close(fd); return 1; }

    // connect to PI
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); goto cleanup_stream; }
    struct sockaddr_in sa = {0};
    sa.sin_family = AF_INET; sa.sin_port = htons(port);
    if (inet_pton(AF_INET, pi_ip, &sa.sin_addr) <= 0) { perror("inet_pton"); close(sock); goto cleanup_stream; }
    if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0) { perror("connect"); close(sock); goto cleanup_stream; }
    fprintf(stderr, "Connected to %s:%d\n", pi_ip, port);

    uint8_t *gray = malloc(FRAME_BYTES);
    if (!gray) { perror("malloc"); close(sock); goto cleanup_stream; }

    // capture loop
    while (1) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            if (errno == EAGAIN) { usleep(1000); continue; }
            perror("VIDIOC_DQBUF"); break;
        }

        // buf.index contains filled buffer
        uint8_t *frame_data = buffers[buf.index].start;
        // convert YUYV->grayscale + resize
        resize_yuyv_to_gray(frame_data, src_w, src_h, gray);

        // send length (big endian) and data
        uint32_t len_be = htonl((uint32_t)FRAME_BYTES);
        if (send(sock, &len_be, 4, 0) != 4) { perror("send len"); break; }
        size_t sent = 0;
        while (sent < FRAME_BYTES) {
            ssize_t s = send(sock, gray + sent, FRAME_BYTES - sent, 0);
            if (s <= 0) { perror("send data"); goto sender_done; }
            sent += s;
        }

        // re-queue buffer
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); break; }

        // throttle if you want lower FPS
        // nanosleep small sleep for ~30 FPS control (adjust)
        struct timespec ts = {0, 33333333}; // 33ms
        nanosleep(&ts, NULL);
    }

sender_done:
    free(gray);
    close(sock);

cleanup_stream:
    // stop streaming and unmap
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < req.count; ++i) {
        munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers);
    close(fd);
    return 0;
}

