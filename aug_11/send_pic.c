#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdint.h>
#include <sys/stat.h>

int uart_init(const char *dev, speed_t baud) {
    int fd = open(dev, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("UART open failed");
        return -1;
    }

    struct termios tty;
    tcgetattr(fd, &tty);

    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);
    tty.c_iflag = tty.c_oflag = tty.c_lflag = 0;

    tcsetattr(fd, TCSANOW, &tty);

    return fd;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <serial_device> <image_file>\n", argv[0]);
        return 1;
    }

    const char *serial_dev = argv[1];
    const char *image_file = argv[2];

    // Open image file
    FILE *fp = fopen(image_file, "rb");
    if (!fp) {
        perror("File open failed");
        return 1;
    }

    // Get file size
    struct stat st;
    if (stat(image_file, &st) != 0) {
        perror("stat failed");
        fclose(fp);
        return 1;
    }
    uint32_t size = st.st_size;

    // Read image data
    unsigned char *buffer = malloc(size);
    if (!buffer) {
        perror("malloc failed");
        fclose(fp);
        return 1;
    }
    fread(buffer, 1, size, fp);
    fclose(fp);

    // Open UART
    int uart_fd = uart_init(serial_dev, B115200);
    if (uart_fd < 0) {
        free(buffer);
        return 1;
    }

    // Send file size (little endian)
    write(uart_fd, &size, sizeof(size));

    // Send image data
    size_t sent = 0;
    while (sent < size) {
        ssize_t n = write(uart_fd, buffer + sent, size - sent);
        if (n > 0) {
            sent += n;
        }
    }

    printf("Sent %u bytes to Raspberry Pi\n", size);

    free(buffer);
    close(uart_fd);
    return 0;
}

