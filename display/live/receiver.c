// uart_receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

#define CHUNK 4096

int open_serial(const char *dev, speed_t baud) {
    int fd = open(dev, O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    cfmakeraw(&tty);
    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CRTSCTS; // disable HW flow control (enable if needed)
    tty.c_cflag &= ~CSTOPB;  // 1 stop bit
    tty.c_cflag &= ~PARENB;  // no parity
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      // 8 data bits

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }
    return fd;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <serial_dev> <output_file.mp4>\n", argv[0]);
        return 1;
    }

    const char *serialdev = argv[1];
    const char *outfile = argv[2];

    int sfd = open_serial(serialdev, B921600);
    if (sfd < 0) return 1;

    int f = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (f < 0) {
        perror("open output file");
        close(sfd);
        return 1;
    }

    char buf[CHUNK];
    ssize_t r;
    while (1) {
        r = read(sfd, buf, sizeof(buf));
        if (r > 0) {
            write(f, buf, r);
            fsync(f);  // flush to disk so player can see it
        } else if (r < 0) {
            perror("read");
            break;
        } else {
            usleep(1000);
        }
    }

    close(f);
    close(sfd);
    return 0;
}

