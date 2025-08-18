// v4l2_h265_x265.c
// Minimal V4L2 capture -> YUYV to I420 -> H.265 (libx265) encoder pipeline.
// Writes raw HEVC bitstream to output.h265
// Compile: gcc -O2 -Wall v4l2_h265_x265.c -o v4l2_h265_x265 -lx265

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <time.h>

#include <x265.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define REQ_BUFS 4

static int xioctl(int fd, int request, void *arg) {
    int r;
    do { r = ioctl(fd, request, arg); }
    while (r == -1 && errno == EINTR);
    return r;
}

struct buffer {
    void   *start;
    size_t  length;
};

static void yuyv_to_i420(const uint8_t *src, int width, int height,
                         uint8_t *dst_y, uint8_t *dst_u, uint8_t *dst_y2, uint8_t *dst_v)
{
    // NOTE: dst_y2 is actually dst_y + width*height; we pass it for pointer math clarity
    // YUYV packs as Y0 U0 Y1 V0 (per 2 pixels). We downsample U/V 2x2 (nearest).
    // This is a very basic converter for demo purposes (no filtering).
    (void)dst_y2; // not used directly

    int y_stride = width;
    int uv_stride = width / 2;

    // Fill Y
    const uint8_t *p = src;
    uint8_t *Y = dst_y;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; x += 2) {
            *Y++ = p[0];     // Y0
            *Y++ = p[2];     // Y1
            p += 4;          // advance YUYV pair
        }
    }

    // Reset pointer and compute U/V by 2x2 subsampling
    p = src;
    uint8_t *U = dst_u;
    uint8_t *V = dst_v;

    for (int y = 0; y < height; y += 2) {
        const uint8_t *row0 = src + y * width * 2;
        const uint8_t *row1 = src + (y + 1) * width * 2;
        for (int x = 0; x < width; x += 2) {
            // Take U,V from the top-left pixel pair (nearest)
            // row: Y0 U0 Y1 V0
            int idx = x * 2;
            uint8_t U0 = row0[idx + 1];
            uint8_t V0 = row0[idx + 3];
            *U++ = U0;
            *V++ = V0;
        }
    }
}

int main(int argc, char **argv) {
    const char *dev = (argc > 1) ? argv[1] : "/dev/video0";
    int width  = (argc > 2) ? atoi(argv[2]) : 640;
    int height = (argc > 3) ? atoi(argv[3]) : 480;
    int frames_to_capture = (argc > 4) ? atoi(argv[4]) : 60;

    // 1) Open V4L2 device
    int fd = open(dev, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // 2) Query capabilities
    struct v4l2_capability cap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP");
        return 1;
    }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
        !(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "Device does not support capture/streaming\n");
        return 1;
    }

    // 3) Set format to YUYV
    struct v4l2_format fmt;
    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // request YUYV
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return 1;
    }
    // Use the negotiated width/height (driver may adjust)
    width = fmt.fmt.pix.width;
    height = fmt.fmt.pix.height;

    // 4) Request MMAP buffers
    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count = REQ_BUFS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        return 1;
    }
    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n", dev);
        return 1;
    }

    struct buffer *buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        return 1;
    }

    for (unsigned i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            return 1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            perror("mmap");
            return 1;
        }
    }

    // 5) Queue buffers
    for (unsigned i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            return 1;
        }
    }

    // 6) Start streaming
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        return 1;
    }

    // 7) Initialize x265
    x265_param *param = x265_param_alloc();
    x265_param_default(param);
    param->sourceWidth  = width;
    param->sourceHeight = height;
    param->fpsNum = 30; param->fpsDenom = 1;             // assume 30 fps
    param->internalCsp = X265_CSP_I420;                  // we feed I420
    x265_param_parse(param, "preset", "ultrafast");      // easier to run realtime
    x265_param_parse(param, "tune", "zerolatency");
    x265_param_parse(param, "bitrate", "2000");          // ~2 Mbps target (Kbps units)

    x265_encoder *encoder = x265_encoder_open(param);
    if (!encoder) {
        fprintf(stderr, "x265_encoder_open failed\n");
        return 1;
    }

    x265_picture *pic_in = x265_picture_alloc();
    x265_picture_init(param, pic_in);

    // Allocate I420 planes
    size_t y_size = (size_t)width * height;
    size_t u_size = y_size / 4;
    size_t v_size = u_size;
    uint8_t *y_plane = (uint8_t*)malloc(y_size);
    uint8_t *u_plane = (uint8_t*)malloc(u_size);
    uint8_t *v_plane = (uint8_t*)malloc(v_size);
    if (!y_plane || !u_plane || !v_plane) {
        fprintf(stderr, "Failed to allocate I420 buffers\n");
        return 1;
    }
    pic_in->planes[0] = y_plane;
    pic_in->planes[1] = u_plane;
    pic_in->planes[2] = v_plane;
    pic_in->stride[0] = width;
    pic_in->stride[1] = width / 2;
    pic_in->stride[2] = width / 2;

    FILE *fout = fopen("output.h265", "wb");
    if (!fout) {
        perror("fopen output.h265");
        return 1;
    }

    // 8) Capture + encode loop
    int frames_encoded = 0;
    while (frames_encoded < frames_to_capture) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        struct timeval tv = { .tv_sec = 2, .tv_usec = 0 };
        int r = select(fd + 1, &fds, NULL, NULL, &tv);
        if (r == -1) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }
        if (r == 0) {
            fprintf(stderr, "select timeout\n");
            continue;
        }

        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            if (errno == EAGAIN) continue;
            perror("VIDIOC_DQBUF");
            break;
        }

        // Convert YUYV -> I420
        yuyv_to_i420((const uint8_t*)buffers[buf.index].start, width, height,
                     y_plane, u_plane, y_plane + y_size, v_plane);

        // Encode 1 frame
        x265_nal *nals = NULL;
        uint32_t nal_count = 0;
        pic_in->pts = frames_encoded;
        int bytes = x265_encoder_encode(encoder, &nals, &nal_count, pic_in, NULL);
        if (bytes < 0) {
            fprintf(stderr, "x265 encode error\n");
            break;
        }
        for (uint32_t i = 0; i < nal_count; ++i) {
            fwrite(nals[i].payload, 1, nals[i].sizeBytes, fout);
        }

        frames_encoded++;

        // Re-queue the buffer
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF (requeue)");
            break;
        }
    }

    // 9) Flush encoder
    {
        x265_nal *nals = NULL;
        uint32_t nal_count = 0;
        while (x265_encoder_encode(encoder, &nals, &nal_count, NULL, NULL) > 0) {
            for (uint32_t i = 0; i < nal_count; ++i) {
                fwrite(nals[i].payload, 1, nals[i].sizeBytes, fout);
            }
        }
    }

    // 10) Cleanup
    fclose(fout);
    x265_encoder_close(encoder);
    x265_picture_free(pic_in);
    x265_param_free(param);

    enum v4l2_buf_type off = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMOFF, &off);
    for (unsigned i = 0; i < req.count; i++) {
        munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers);
    free(y_plane); free(u_plane); free(v_plane);
    close(fd);

    fprintf(stderr, "Done. Wrote H.265 bitstream to output.h265\n");
    return 0;
}
