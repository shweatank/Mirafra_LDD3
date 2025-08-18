// gcc -O2 capture_send_uart.c -o capture_send_uart
// sudo ./capture_send_uart /dev/video0 /dev/ttyUSB0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <errno.h>
#include<stdint.h>

struct buffer {
    void *start;
    size_t length;
};

int xioctl(int fd, int request, void *arg) {
    int r;
    do { r = ioctl(fd, request, arg); } while (r == -1 && errno == EINTR);
    return r;
}

int uart_init(const char *dev, speed_t baud) {
    int fd = open(dev, O_RDWR | O_NOCTTY);
    if (fd < 0) return -1;
    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag = tty.c_oflag = tty.c_lflag = 0;
    tcsetattr(fd, TCSANOW, &tty);
    return fd;
}

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "Usage: %s <video_device> <uart_device>\n", argv[0]); return 1; }
    const char *dev_name = argv[1];
    const char *uart_dev = argv[2];

    // Open UART
    int uart_fd = uart_init(uart_dev, B115200);
    if (uart_fd < 0) { perror("UART open"); return 1; }

    // Open camera
    int fd = open(dev_name, O_RDWR);
    if (fd < 0) { perror("Camera open"); return 1; }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 320;
    fmt.fmt.pix.height = 240;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    xioctl(fd, VIDIOC_S_FMT, &fmt);

    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    xioctl(fd, VIDIOC_REQBUFS, &req);

    struct v4l2_buffer buf = {0};
    buf.type = req.type; buf.memory = V4L2_MEMORY_MMAP; buf.index = 0;
    xioctl(fd, VIDIOC_QUERYBUF, &buf);
    void *buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    xioctl(fd, VIDIOC_QBUF, &buf);

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMON, &type);

    // Capture one frame
    xioctl(fd, VIDIOC_DQBUF, &buf);

    // Send size then data
    uint32_t size = buf.bytesused;
    write(uart_fd, &size, sizeof(size));
    write(uart_fd, buffer, size);

    munmap(buffer, buf.length);
    close(fd);
    close(uart_fd);
    return 0;
}

