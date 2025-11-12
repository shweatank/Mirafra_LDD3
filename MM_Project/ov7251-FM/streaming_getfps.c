#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>
#include <sys/time.h>

#define DEVICE          "/dev/video0"
#define WIDTH           640
#define HEIGHT          480
#define NUM_BUFFERS     4
#define CAPTURE_TIME_SEC 5
#define FRAME_DIR       "frames"

struct buffer {
    void   *start;
    size_t  length;
};

struct buffer *buffers;

// Function to save GREY8 frame as PGM
void save_frame_pgm(uint8_t *data, int width, int height, int index) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/frame_%04d.pgm", FRAME_DIR, index);

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        return;
    }

    fprintf(fp, "P5\n%d %d\n255\n", width, height);
    fwrite(data, 1, width*height, fp);
    fclose(fp);
}

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Cannot open device");
        return 1;
    }

    // Create frames directory if it doesn't exist
    mkdir(FRAME_DIR, 0777);

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
            return 1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length,
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                fd, buf.m.offset);
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

    int running = 1;
    int frame_count = 0;
    struct timeval start_time, current_time, prev_timestamp, cur_timestamp;
    int first_frame = 1;

    gettimeofday(&start_time, NULL);

    while (running) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("VIDIOC_DQBUF");
            break;
        }

        // Count unique frames using timestamps
        cur_timestamp = buf.timestamp;
        if (first_frame || (cur_timestamp.tv_sec != prev_timestamp.tv_sec ||
                            cur_timestamp.tv_usec != prev_timestamp.tv_usec)) {
            frame_count++;
            prev_timestamp = cur_timestamp;
            first_frame = 0;

            // Save frame
            save_frame_pgm(buffers[buf.index].start, WIDTH, HEIGHT, frame_count);
        }

        // Re-queue buffer
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            break;
        }

        gettimeofday(&current_time, NULL);
        double elapsed = (current_time.tv_sec - start_time.tv_sec) +
                         (current_time.tv_usec - start_time.tv_usec) / 1e6;
        if (elapsed >= CAPTURE_TIME_SEC)
            running = 0;
    }

    double fps = frame_count / (double)CAPTURE_TIME_SEC;
    printf("Total unique frames captured in %d seconds: %d\n", CAPTURE_TIME_SEC, frame_count);
    printf("Approx FPS: %.2f\n", fps);

    // Cleanup
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (int i = 0; i < req.count; i++)
        munmap(buffers[i].start, buffers[i].length);
    free(buffers);
    close(fd);

    return 0;
}

