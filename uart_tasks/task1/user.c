#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int main() {
    int uart0 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0 < 0) {
        perror("UART open failed");
        return 1;
    }

    struct termios options;

    // Get current settings
    tcgetattr(uart0, &options);

    // Set baud rates to 115200
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    // 8 data bits, no parity, 1 stop bit (8N1)
    options.c_cflag &= ~PARENB; // No parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;     // 8 data bits

    options.c_cflag |= (CLOCAL | CREAD); // Enable receiver, ignore modem control lines

    options.c_lflag = 0; // Raw input mode
    options.c_iflag = 0; // No software flow control
    options.c_oflag = 0; // Raw output

    options.c_cc[VMIN] = 1;  // Read at least 1 character
    options.c_cc[VTIME] = 0; // No timeout

    // Apply settings
    tcsetattr(uart0, TCSANOW, &options);

    const char *msg = "Hello UART from Laptop\n";
    int len = strlen(msg);

    int written = write(uart0, msg, len);
    if (written < 0) {
        perror("UART write failed");
    } else {
        printf("Sent %d bytes: %s", written, msg);
    }

    close(uart0);
    return 0;
}

