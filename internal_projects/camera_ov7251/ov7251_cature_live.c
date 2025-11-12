/ SPDX-License-Identifier: GPL-2.0
// OV7251 live preview using V4L2 + OpenCV (10-bit or 8-bit greyscale)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <time.h>
#include <opencv2/opencv.hpp>

#define DEVICE "/dev/video0"
#define WIDTH 640
#define HEIGHT 480
#define BUFFER_COUNT 4
#define CAPTURE_DURATION 10   // seconds

using namespace cv;

struct buffer {
    void *start;
    size_t length;
};

int main(void) {
    int fd;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;
    struct buffer buffers[BUFFER_COUNT];
    fd_set fds;
    struct timeval tv;
    time_t start_time;
    int frame = 0;

    // Open camera device
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Set camera format
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;  // or V4L2_PIX_FMT_GREY
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }

    // Request buffers
    memset(&req, 0, sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        close(fd);
        return 1;
    }

    // Map buffers
    for (int i = 0; i < BUFFER_COUNT; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            close(fd);
            return 1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            perror("mmap");
            close(fd);
            return 1;
        }

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            close(fd);
            return 1;
        }
    }

    // Start streaming
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        close(fd);
        return 1;
    }

    printf("Displaying live video for %d seconds... (press 'q' to quit)\n", CAPTURE_DURATION);
    start_time = time(NULL);

    while (difftime(time(NULL), start_time) < CAPTURE_DURATION) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        int r = select(fd + 1, &fds, NULL, NULL, &tv);
        if (r == -1) {
            perror("select");
            break;
        }

        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("VIDIOC_DQBUF");
            break;
        }

        // Convert buffer to OpenCV Mat
        Mat frame10(HEIGHT, WIDTH, CV_16UC1, buffers[buf.index].start);
        Mat frame8;
        frame10.convertTo(frame8, CV_8UC1, 1.0 / 4.0); // scale 10-bit -> 8-bit for display

        imshow("OV7251 Live", frame8);
        if (waitKey(1) == 'q')
            break;

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            break;
        }
    }

    // Stop streaming
    ioctl(fd, VIDIOC_STREAMOFF, &type);

    // Cleanup
    for (int i = 0; i < BUFFER_COUNT; i++)
        munmap(buffers[i].start, buffers[i].length);

    close(fd);
    destroyAllWindows();

    printf("Stopped.\n");
    return 0;
}

