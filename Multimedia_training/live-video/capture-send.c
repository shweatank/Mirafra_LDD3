#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define SRC_W 1280
#define SRC_H 720
#define OUT_W 128
#define OUT_H 32
#define OUT_BYTES ((OUT_W * OUT_H) / 8) // 512

struct v4l_buf { void *start; size_t len; };

static uint16_t crc16(const uint8_t *data, size_t n) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < n; ++i) {
        crc ^= data[i];
        for (int b=0;b<8;b++) {
            if (crc & 1) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }
    return crc;
}

static int set_serial_speed(int fd, int baud) {
    struct termios tty;
    if (tcgetattr(fd, &tty) < 0) return -1;
    cfmakeraw(&tty);

    speed_t sp = B115200;
    if (baud == 115200) sp = B115200;
    else if (baud == 230400) sp = B230400;
#ifdef B460800
    else if (baud == 460800) sp = B460800;
#endif
#ifdef B921600
    else if (baud == 921600) sp = B921600;
#endif

    cfsetospeed(&tty, sp);
    cfsetispeed(&tty, sp);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE; tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS;
#endif
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK);
    tty.c_lflag = 0; tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1; tty.c_cc[VTIME] = 0;
    return tcsetattr(fd, TCSANOW, &tty);
}

static ssize_t full_write(int fd, const void *buf, size_t count) {
    const uint8_t *p = buf; size_t left = count;
    while (left) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        left -= w; p += w;
    }
    return (ssize_t)count;
}

static ssize_t full_read(int fd, void *buf, size_t count) {
    uint8_t *p = buf; size_t left = count;
    while (left) {
        ssize_t r = read(fd, p, left);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        } else if (r == 0) return (ssize_t)(count - left);
        left -= r; p += r;
    }
    return (ssize_t)count;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr,"Usage: %s /dev/videoX /dev/ttyUSBY baud\n", argv[0]);
        return 1;
    }
    const char *video_dev = argv[1];
    const char *serial_dev = argv[2];
    int baud = atoi(argv[3]);

    // Open camera
    int vd = open(video_dev, O_RDWR | O_NONBLOCK);
    if (vd < 0) { perror("open video"); return 1; }

    // set requested format (try 1280x720 YUYV)
    struct v4l2_format fmt; memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = SRC_W;
    fmt.fmt.pix.height = SRC_H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(vd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT (requested)");
        if (ioctl(vd, VIDIOC_G_FMT, &fmt) < 0) { perror("VIDIOC_G_FMT"); close(vd); return 1; }
    }
    int in_w = fmt.fmt.pix.width, in_h = fmt.fmt.pix.height;
    fprintf(stderr,"Camera resolution: %dx%d, pixfmt: %c%c%c%c\n", in_w, in_h,
            fmt.fmt.pix.pixelformat & 0xFF,
            (fmt.fmt.pix.pixelformat>>8)&0xFF,(fmt.fmt.pix.pixelformat>>16)&0xFF,(fmt.fmt.pix.pixelformat>>24)&0xFF);

    // request buffers (mmap)
    struct v4l2_requestbuffers req; memset(&req,0,sizeof(req));
    req.count = 4; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(vd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); close(vd); return 1; }
    struct v4l_buf *vbufs = calloc(req.count, sizeof(struct v4l_buf));
    for (int i=0;i<(int)req.count;i++) {
        struct v4l2_buffer b; memset(&b,0,sizeof(b));
        b.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; b.memory = V4L2_MEMORY_MMAP; b.index = i;
        if (ioctl(vd, VIDIOC_QUERYBUF, &b) < 0) { perror("VIDIOC_QUERYBUF"); close(vd); return 1; }
        vbufs[i].len = b.length;
        vbufs[i].start = mmap(NULL, b.length, PROT_READ|PROT_WRITE, MAP_SHARED, vd, b.m.offset);
        if (vbufs[i].start == MAP_FAILED) { perror("mmap"); close(vd); return 1; }
    }
    for (int i=0;i<(int)req.count;i++) {
        struct v4l2_buffer b; memset(&b,0,sizeof(b));
        b.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; b.memory = V4L2_MEMORY_MMAP; b.index = i;
        if (ioctl(vd, VIDIOC_QBUF, &b) < 0) { perror("VIDIOC_QBUF"); close(vd); return 1; }
    }
    int t = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(vd, VIDIOC_STREAMON, &t) < 0) { perror("STREAMON"); close(vd); return 1; }

    // open serial
    int sd = open(serial_dev, O_RDWR | O_NOCTTY | O_SYNC);
    if (sd < 0) { perror("open serial"); return 1; }
    if (set_serial_speed(sd, baud) < 0) { perror("set_serial_speed"); /* continue */ }

    // compute aspect-fit cropping on source
    double src_as = (double)in_w / in_h;
    double dst_as = (double)OUT_W / OUT_H;
    int crop_x=0, crop_y=0, crop_w=in_w, crop_h=in_h;
    if (src_as > dst_as) {
        // crop width (pillarbox) -> center horizontally
        crop_w = (int)(in_h * dst_as + 0.5);
        crop_x = (in_w - crop_w)/2;
    } else {
        // crop height (letterbox)
        crop_h = (int)(in_w / dst_as + 0.5);
        crop_y = (in_h - crop_h)/2;
    }

    fprintf(stderr,"Crop src: x=%d y=%d w=%d h=%d\n", crop_x, crop_y, crop_w, crop_h);

    uint8_t outbuf[OUT_BYTES];
    uint8_t gray[OUT_H][OUT_W];

    uint32_t seq = 0;
    while (1) {
        // wait for single-byte 'R' from Pi meaning ready
        uint8_t rch;
        ssize_t r = full_read(sd, &rch, 1);
        if (r != 1) { usleep(2000); continue; }
        if (rch != 'R') {
            // ignore unexpected bytes
            continue;
        }

        // Dequeue buffer
        struct v4l2_buffer vb; memset(&vb,0,sizeof(vb));
        vb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; vb.memory = V4L2_MEMORY_MMAP;
        if (ioctl(vd, VIDIOC_DQBUF, &vb) < 0) {
            if (errno == EAGAIN) { usleep(1000); continue; }
            perror("VIDIOC_DQBUF"); break;
        }
        uint8_t *frame = (uint8_t*)vbufs[vb.index].start;
        size_t framesz = vbufs[vb.index].len;

        // fill gray with background 0
        for (int y=0;y<OUT_H;y++) for (int x=0;x<OUT_W;x++) gray[y][x]=0;

        // nearest-neighbor scaling mapping dst -> src (use crop)
        for (int dy=0; dy<OUT_H; dy++) {
            for (int dx=0; dx<OUT_W; dx++) {
                // map to source coordinates within crop
                int sx = crop_x + (dx * crop_w) / OUT_W;
                int sy = crop_y + (dy * crop_h) / OUT_H;
                if (sx < 0) sx = 0; if (sx >= in_w) sx = in_w-1;
                if (sy < 0) sy = 0; if (sy >= in_h) sy = in_h-1;
                // YUYV: for pixel index p = sy*in_w + sx
                size_t pidx = (size_t)(sy * in_w + sx) * 2;
                uint8_t Y = 0;
                if (pidx < framesz) Y = frame[pidx]; // Y is at even bytes
                gray[dy][dx] = Y;
            }
        }

        // pack into SSD1306 page order (page0 rows0..7, etc.)
        memset(outbuf, 0, OUT_BYTES);
        for (int page=0; page < OUT_H/8; page++) {
            for (int x=0; x<OUT_W; x++) {
                uint8_t b = 0;
                for (int bit=0; bit<8; bit++) {
                    int y = page*8 + bit;
                    uint8_t lum = gray[y][x];
                    int on = (lum > 128) ? 1 : 0;
                    if (on) b |= (1u << bit); // bit0 = top pixel in this 8-vertical block
                }
                outbuf[page*OUT_W + x] = b;
            }
        }

        // build header seq(4 big-endian) len(2 big-endian)
        uint8_t header[10];
        header[0]=0xAA; header[1]=0x55; header[2]=0xAA; header[3]=0x55;
        header[4]=(seq>>24)&0xFF; header[5]=(seq>>16)&0xFF; header[6]=(seq>>8)&0xFF; header[7]=seq&0xFF;
        header[8]= (OUT_BYTES>>8)&0xFF; header[9]= OUT_BYTES & 0xFF;

        uint8_t crcbuf[6 + OUT_BYTES];
        memcpy(crcbuf, &header[4], 6); // seq + len
        memcpy(crcbuf + 6, outbuf, OUT_BYTES);
        uint16_t crc = crc16(crcbuf, sizeof(crcbuf));

        // send header, payload, crc (crc little-endian)
        if (full_write(sd, header, sizeof(header)) < 0) perror("write header");
        if (full_write(sd, outbuf, OUT_BYTES) < 0) perror("write payload");
        uint8_t crcbytes[2] = { crc & 0xFF, (crc >> 8) & 0xFF };
        if (full_write(sd, crcbytes, 2) < 0) perror("write crc");

        seq++;
        // requeue vb
        if (ioctl(vd, VIDIOC_QBUF, &vb) < 0) perror("VIDIOC_QBUF");
    }

    // cleanup (not reached normally)
    t = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(vd, VIDIOC_STREAMOFF, &t);
    for (int i=0;i<(int)req.count;i++) munmap(vbufs[i].start, vbufs[i].len);
    close(vd); close(sd);
    free(vbufs);
    return 0;
}

