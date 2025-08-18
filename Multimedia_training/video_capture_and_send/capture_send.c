// host_capture.c
// Compile: gcc -O2 -Wall host_capture.c -o host_capture
// Run:   sudo ./host_capture /dev/video0 /dev/ttyUSB0 921600
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <termios.h>
#include <stdint.h>

#define OUT_W 128
#define OUT_H 32
#define OUT_BYTES ((OUT_W*OUT_H)/8) // 512

struct buffer {
    void *start;
    size_t length;
};

static int set_serial_speed(int fd, int baud) {
    struct termios tty;
    if (tcgetattr(fd, &tty) < 0) return -1;
    cfmakeraw(&tty);
    // try to set common high speeds via termios constants if available
    speed_t sp;
    switch (baud) {
        case 115200: sp = B115200; break;
        case 230400: sp = B230400; break;
        case 460800: sp = B460800; break;
#ifdef B921600
        case 921600: sp = B921600; break;
#endif
        default: sp = B115200; break;
    }
    cfsetospeed(&tty, sp);
    cfsetispeed(&tty, sp);
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSANOW, &tty) < 0) return -1;
    return 0;
}

static uint16_t crc16(const uint8_t *data, size_t n) {
    uint16_t crc = 0xFFFF;
    for (size_t i=0;i<n;i++) {
        crc ^= data[i];
        for (int k=0;k<8;k++) {
            if (crc & 1) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }
    return crc;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s /dev/videoX /dev/ttyUSBY baud\n", argv[0]);
        return 1;
    }
    const char *video_dev = argv[1];
    const char *serial_dev = argv[2];
    int baud = atoi(argv[3]);

    // Open camera
    int vd = open(video_dev, O_RDWR);
    if (vd < 0) { perror("open video"); return 1; }

    // Query capabilities
    struct v4l2_capability cap;
    if (ioctl(vd, VIDIOC_QUERYCAP, &cap) < 0) { perror("VIDIOC_QUERYCAP"); close(vd); return 1; }
    // Set format (try 640x480 YUYV)
    struct v4l2_format fmt;
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(vd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); /* continue */ }

    // Request buffers
    struct v4l2_requestbuffers req;
    memset(&req,0,sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(vd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); close(vd); return 1; }
    struct buffer *buffers = calloc(req.count, sizeof(struct buffer));
    for (size_t i=0;i<req.count;i++) {
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(vd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); close(vd); return 1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, vd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) { perror("mmap"); close(vd); return 1; }
    }
    for (size_t i=0;i<req.count;i++) {
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        ioctl(vd, VIDIOC_QBUF, &buf);
    }
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(vd, VIDIOC_STREAMON, &type);

    // Open serial
    int sd = open(serial_dev, O_RDWR | O_NOCTTY | O_SYNC);
    if (sd < 0) { perror("open serial"); return 1; }
    if (set_serial_speed(sd, baud) < 0) { perror("set_serial_speed (maybe unsupported)"); /* continue */ }

    // Buffers
    uint8_t outbits[OUT_BYTES];
    uint8_t tmpbuf[OUT_W * OUT_H]; // temporary 1 byte per pixel after downscale

    uint32_t seq = 0;
    while (1) {
        // Dequeue
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(vd, VIDIOC_DQBUF, &buf) < 0) { perror("VIDIOC_DQBUF"); break; }
        // framesize: use fmt.fmt.pix.width/height
        unsigned char *data = (unsigned char*)buffers[buf.index].start;
        int in_w = fmt.fmt.pix.width;
        int in_h = fmt.fmt.pix.height;

        // convert YUYV -> grayscale and downscale to OUT_W x OUT_H (nearest neighbor)
        for (int y=0;y<OUT_H;y++) {
            for (int x=0;x<OUT_W;x++) {
                int srcx = x * in_w / OUT_W;
                int srcy = y * in_h / OUT_H;
                int pos = (srcy * in_w + srcx) * 2; // YUYV: Y at even bytes
                if (pos+1 >= (int)buffers[buf.index].length) pos = 0;
                unsigned char Y = data[pos];
                tmpbuf[y*OUT_W + x] = Y; // 0-255
            }
        }

        // simple threshold to 1-bit (you can change to better dithering)
        for (size_t i=0;i<OUT_BYTES;i++) outbits[i]=0;
        for (int y=0;y<OUT_H;y++) {
            for (int x=0;x<OUT_W;x++) {
                int v = tmpbuf[y*OUT_W + x];
                int bit = (v > 128) ? 1 : 0;
                size_t index = y*OUT_W + x;
                if (bit) outbits[index/8] |= (1 << (7 - (index % 8)));
            }
        }

        // Build header: 0xAA55AA55 (4B) | seq (4B) | len (2B) | payload (512B) | crc16 (2B)
        uint8_t header[10];
        header[0]=0xAA; header[1]=0x55; header[2]=0xAA; header[3]=0x55;
        header[4]= (seq >> 24) & 0xFF;
        header[5]= (seq >> 16) & 0xFF;
        header[6]= (seq >> 8) & 0xFF;
        header[7]= (seq) & 0xFF;
        header[8]= (OUT_BYTES >> 8) & 0xFF;
        header[9]= (OUT_BYTES) & 0xFF;

        // compute crc over header+payload
        uint8_t packet[10 + OUT_BYTES];
        memcpy(packet, header, 10);
        memcpy(packet+10, outbits, OUT_BYTES);
        uint16_t crc = crc16(packet, sizeof(packet));

        // write to serial (blocking), attempt single write for atomic-ish send
        ssize_t w;
        w = write(sd, packet, sizeof(packet));
        if (w != (ssize_t)sizeof(packet)) {
            // try again in loop
            ssize_t tot= w>0?w:0;
            while (tot < (ssize_t)sizeof(packet)) {
                ssize_t ww = write(sd, ((uint8_t*)packet)+tot, sizeof(packet)-tot);
                if (ww <= 0) break;
                tot += ww;
            }
        }
        // write crc
        uint8_t crbb[2] = { crc & 0xFF, (crc>>8)&0xFF };
        write(sd, crbb, 2);

        seq++;

        // requeue buffer
        ioctl(vd, VIDIOC_QBUF, &buf);
        // small usleep to avoid 100% CPU; adjust as needed
        usleep(1000);
    }

    // cleanup
    for (size_t i=0;i<req.count;i++) munmap(buffers[i].start, buffers[i].length);
    free(buffers);
    close(vd);
    close(sd);
    return 0;
}

