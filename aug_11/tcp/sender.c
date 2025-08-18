// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <sys/ioctl.h>  
#define WIDTH 640
#define HEIGHT 480

struct buffer {
    void *start;
    size_t length;
};

int xioctl(int fd, int request, void *arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (r == -1 && errno == EINTR);
    return r;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <RaspberryPi IP> <port>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // Open webcam device
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0) {
        perror("Opening video device");
        return 1;
    }

    // Query capabilities
    struct v4l2_capability cap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP");
        return 1;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "Device does not support capture\n");
        return 1;
    }

    // Set format to MJPEG
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return 1;
    }

    // Request buffers
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        return 1;
    }

    // Map buffers
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
        perror("VIDIOC_QUERYBUF");
        return 1;
    }

    struct buffer buffer_start;
    buffer_start.length = buf.length;
    buffer_start.start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    if (buffer_start.start == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Queue buffer
    if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) {
        perror("VIDIOC_QBUF");
        return 1;
    }

    // Start streaming
    int type = buf.type;
    if (xioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        return 1;
    }

    // Setup TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address\n");
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return 1;
    }
    printf("Connected to Raspberry Pi\n");

    while (1) {
        // Dequeue buffer to get frame
        if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("VIDIOC_DQBUF");
            break;
        }

        // Send frame size (uint32_t in network byte order)
        uint32_t frame_size = htonl(buf.bytesused);
        if (write(sock, &frame_size, sizeof(frame_size)) != sizeof(frame_size)) {
            perror("write size");
            break;
        }

        // Send MJPEG data
        ssize_t sent = 0;
        while (sent < buf.bytesused) {
            ssize_t n = write(sock, buffer_start.start + sent, buf.bytesused - sent);
            if (n <= 0) {
                perror("write data");
                goto cleanup;
            }
            sent += n;
        }

        printf("Sent frame size %u bytes\n", buf.bytesused);

        // Re-queue buffer
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            break;
        }
    }

cleanup:
    // Stop streaming
    xioctl(fd, VIDIOC_STREAMOFF, &type);
    munmap(buffer_start.start, buffer_start.length);
    close(fd);
    close(sock);
    return 0;
}

