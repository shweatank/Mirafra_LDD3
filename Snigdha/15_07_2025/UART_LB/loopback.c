#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int main() {
    int uart0 = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
    if (uart0 < 0) {
        perror("UART open failed");
        return 1;
    }

    struct termios options;
    tcgetattr(uart0, &options);

    // Set input and output Baud rate
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    // 8N1 mode: 8 data bits, no parity, 1 stop bit
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // No flow control
    options.c_cflag &= ~CRTSCTS;

    // Raw input/output
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    options.c_cflag |= (CLOCAL | CREAD);

    tcsetattr(uart0, TCSANOW, &options);

    // Write to UART
    const char *msg = "Hello UART\n";
    write(uart0, msg, strlen(msg));
    printf("Sent: %s", msg);

    // Read back the response (loopback)
    char buf[100];
    int n = read(uart0, buf, sizeof(buf) - 1);
    if (n < 0) {
        perror("UART read failed");
    } else {
        buf[n] = '\0';
        printf("Received: %s", buf);
    }

    close(uart0);
    return 0;
}

