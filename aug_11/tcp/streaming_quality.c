// sender.c
// Capture webcam → 128x64 → 1-bit OLED → send over TCP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <errno.h>

#define CAM_WIDTH  640
#define CAM_HEIGHT 480
#define OLED_W     128
#define OLED_H     64
#define SERVER_IP  "192.168.1.50"  // Change to your Pi's IP
#define SERVER_PORT 9000

struct buffer {
    void   *start;
    size_t length;
};

// Scale RGB888 to 1-bit OLED buffer (SSD1306 format)
void scale_and_dither(uint8_t *rgb, uint8_t *oled_buf) {
    memset(oled_buf, 0, OLED_W * (OLED_H / 8));
    for (int y = 0; y < OLED_H; y++) {
        for (int x = 0; x < OLED_W; x++) {
            int srcX = x * CAM_WIDTH / OLED_W;
            int srcY = y * CAM_HEIGHT / OLED_H;
            uint8_t *pix = rgb + (srcY * CAM_WIDTH + srcX) * 3;
            int gray = (pix[0] * 30 + pix[1] * 59 + pix[2] * 11) / 100; // luminance
            if (gray > 127) {
                oled_buf[x + (y / 8) * OLED_W] |= (1 << (y % 8));
            }
        }
    }
}

int main() {
    int cam_fd = open("/dev/video0", O_RDWR);
    if (cam_fd < 0) { perror("open camera"); return 1; }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = CAM_WIDTH;
    fmt.fmt.pix.height = CAM_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(cam_fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return 1; }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(cam_fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); return 1; }

    struct buffer buf;
    struct v4l2_buffer vbuf;
    memset(&vbuf, 0, sizeof(vbuf));
    vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vbuf.memory = V4L2_MEMORY_MMAP;
    vbuf.index = 0;

    if (ioctl(cam_fd, VIDIOC_QUERYBUF, &vbuf) < 0) { perror("VIDIOC_QUERYBUF"); return 1; }
    buf.length = vbuf.length;
    buf.start = mmap(NULL, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, cam_fd, vbuf.m.offset);
    if (buf.start == MAP_FAILED) { perror("mmap"); return 1; }

    if (ioctl(cam_fd, VIDIOC_QBUF, &vbuf) < 0) { perror("VIDIOC_QBUF"); return 1; }
    if (ioctl(cam_fd, VIDIOC_STREAMON, &vbuf.type) < 0) { perror("VIDIOC_STREAMON"); return 1; }

    // TCP socket connect to Pi
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return 1;
    }

    printf("Streaming to %s:%d\n", SERVER_IP, SERVER_PORT);

    uint8_t oled_buf[OLED_W * (OLED_H / 8)];

    while (1) {
        if (ioctl(cam_fd, VIDIOC_DQBUF, &vbuf) < 0) { perror("VIDIOC_DQBUF"); return 1; }

        // Convert frame to OLED format
        scale_and_dither(buf.start, oled_buf);

        // Send 1KB frame
        if (send(sock, oled_buf, sizeof(oled_buf), 0) <= 0) {
            perror("send");
            break;
        }

        if (ioctl(cam_fd, VIDIOC_QBUF, &vbuf) < 0) { perror("VIDIOC_QBUF"); return 1; }
    }

    close(sock);
    close(cam_fd);
    return 0;
}

