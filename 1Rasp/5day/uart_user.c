// uart_user.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#define SERIAL_PORT "/dev/ttyUSB1"
#define BAUD_RATE B115200

int configure_port(int fd) {
    struct termios options;

    if (tcgetattr(fd, &options) != 0) {
        perror("tcgetattr");
        return -1;
    }

    // Set baud rate
    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);

    // 8 data bits, no parity, 1 stop bit (8N1)
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Turn on read & ignore modem ctrl lines
    options.c_cflag |= (CLOCAL | CREAD);

    // Raw input/output mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    // Apply settings
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("tcsetattr");
        return -1;
    }

    return 0;
}

int main() {
    int fd;
    char buf[256];
    ssize_t n;
    const char *init_msg = "Hello from x86\n";
    const char *response = "go\n";

    // Open serial port
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("Failed to open serial port");
        exit(EXIT_FAILURE);
    }

    if (configure_port(fd) < 0) {
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Set non-blocking
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Serial port %s opened. Starting communication...\n", SERIAL_PORT);

    while (1) {
        // Read from UART
        n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Received: %s", buf);

            if (strstr(buf, "hi")) {
                n = write(fd, response, strlen(response));
                if (n < 0) {
                    perror("Write error (response)");
                    break;
                }
                printf("Sent: %s", response);
            }
        } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("Read error");
            break;
        }

        // Periodically send a message every 5 seconds
        sleep(5);
        n = write(fd, init_msg, strlen(init_msg));
        if (n < 0) {
            perror("Write error (init_msg)");
            break;
        }
        printf("Sent: %s", init_msg);
    }

    close(fd);
    return 0;
}

