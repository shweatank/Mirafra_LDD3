// v4l2_capture_10sec.c
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

#define DEVICE "/dev/video0"
#define WIDTH 1280
#define HEIGHT 720
#define REQ_BUF_COUNT 4
#define DURATION_SEC 10

struct buffer {
    void   *start;
    size_t  length;
};

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Cannot open device");
        return 1;
    }

    // 1. Query Capabilities
    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP");
        return 1;
    }

    // 2. Set Format
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // Raw YUYV
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return 1;
    }

    // 3. Try setting FPS = 10 (camera may ignore)
    struct v4l2_streamparm fps = {0};
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fps.parm.capture.timeperframe.numerator = 1;
    fps.parm.capture.timeperframe.denominator = 10; // 10 fps target

    if (ioctl(fd, VIDIOC_S_PARM, &fps) < 0) {
        perror("VIDIOC_S_PARM");
    }

    // 4. Request Buffers
    struct v4l2_requestbuffers req = {0};
    req.count = REQ_BUF_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        return 1;
    }

    // 5. Query + Mmap Buffers
    struct buffer buffers[REQ_BUF_COUNT];
    struct v4l2_buffer buf;
    for (int i = 0; i < REQ_BUF_COUNT; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
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

    // 6. Queue Buffers
    for (int i = 0; i < REQ_BUF_COUNT; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            return 1;
        }
    }

    // 7. Start Streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        return 1;
    }

    FILE *file = fopen("output.yuv", "wb");
    if (!file) {
        perror("fopen");
        return 1;
    }

    // 8. Capture Loop (time-based: 10 sec)
    time_t start = time(NULL);
    while (difftime(time(NULL), start) < DURATION_SEC) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("VIDIOC_DQBUF");
            break;
        }

        fwrite(buffers[buf.index].start, buf.bytesused, 1, file);

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            break;
        }

        printf("Captured frame (index=%d, size=%u bytes)\n", buf.index, buf.bytesused);
    }

    // 9. Stop Streaming
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("VIDIOC_STREAMOFF");
        return 1;
    }

    // 10. Cleanup
    fclose(file);
    for (int i = 0; i < REQ_BUF_COUNT; i++)
        munmap(buffers[i].start, buffers[i].length);
    close(fd);

    printf("Saved video: %d sec to output.yuv\n", DURATION_SEC);
    return 0;
}


