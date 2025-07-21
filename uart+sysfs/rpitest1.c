#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int main() {
    int uart = open("/dev/serial0", O_RDWR | O_NOCTTY);
    if (uart < 0) {
        perror("UART open failed");
        return 1;
    }

    struct termios options;
    tcgetattr(uart, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD);    // Enable receiver, set local mode
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;                 // 8-bit chars
    options.c_cflag &= ~PARENB;             // No parity
    options.c_cflag &= ~CSTOPB;             // 1 stop bit
    options.c_cflag &= ~CRTSCTS;            // No flow control
    options.c_lflag = 0;                    // Raw input
    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;

    tcsetattr(uart, TCSANOW, &options);

    char buf[100];

    while (1) {
        ssize_t len = read(uart, buf, sizeof(buf) - 1);
        if (len > 0) {
            buf[len] = '\0';
            printf("Received: %s\n", buf);
            
	    sleep(2);
            // Echo back to PC
            write(uart, buf, len);
        }
    }

    close(uart);
    return 0;
}

