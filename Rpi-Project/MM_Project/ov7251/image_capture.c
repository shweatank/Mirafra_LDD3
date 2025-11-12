#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define DEVICE       "/dev/video0"
#define NUM_BUFFERS  4

struct buffer {
    void   *start;
    size_t  length;
};
struct buffer *buffers;

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        perror("Cannot open device");
        return 1;
    }

    // Enumerate supported formats
    struct v4l2_fmtdesc fmtdesc;
    memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

/*    printf("Supported formats:\n");
    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) 
    {
        printf("  %d: %s (fourcc 0x%08x)\n",
               fmtdesc.index, fmtdesc.description, fmtdesc.pixelformat);
        fmtdesc.index++;
    }
*/
    // Set format (try RAW10 Y10 first)
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = 640;
    fmt.fmt.pix.height      = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY; // try Y10
    fmt.fmt.pix.field       = V4L2_FIELD_ANY;

    if(ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) 
    {
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }

    // Get and print what the driver accepted
    if (ioctl(fd, VIDIOC_G_FMT, &fmt) == 0) 
    {
        printf("Using format: %dx%d fourcc=0x%08x\n", fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.pixelformat);
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

    if (req.count < 1) {
        fprintf(stderr, "No buffers allocated\n");
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

    // Dequeue one buffer
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
        perror("VIDIOC_DQBUF");
        return 1;
    }

    printf("Captured frame: %d bytes\n", buf.bytesused);

    // Save to PGM (convert if Y10 -> 8-bit)
    FILE *f = fopen("capture.pgm", "wb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    int width = fmt.fmt.pix.width;
    int height = fmt.fmt.pix.height;

    fprintf(f, "P5\n%d %d\n255\n", width, height);

    if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_Y10) {
        // Y10: 10-bit packed little-endian → unpack to 8-bit
        unsigned char *src = buffers[buf.index].start;
        int num_pixels = width * height;
        int i, j = 0;
        for (i = 0; i < num_pixels * 2; i += 2) {
            unsigned short val = src[i] | (src[i+1] << 8);
            unsigned char out = (val >> 2) & 0xFF; // scale 10-bit to 8-bit
            fwrite(&out, 1, 1, f);
            j++;
            if (j >= num_pixels) break;
        }
    } else {
        // Assume 8-bit greyscale
        fwrite(buffers[buf.index].start, 1, width * height, f);
    }

    fclose(f);
    printf("Saved image to capture.pgm\n");

    // Queue buffer back
    if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
        perror("VIDIOC_QBUF");
    }

    // Stop streaming
    ioctl(fd, VIDIOC_STREAMOFF, &type);

    for (int i = 0; i < req.count; i++)
        munmap(buffers[i].start, buffers[i].length);

    free(buffers);
    close(fd);
    return 0;
}

