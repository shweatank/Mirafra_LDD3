#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main() {
    int uart0 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0 < 0) {
        perror("UART open failed");
        return 1;
    }

    struct termios options;
    tcgetattr(uart0, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag &= ~PARENB; // No parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;     // 8 data bits
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_lflag = 0;
    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;

    tcsetattr(uart0, TCSANOW, &options);

    char ch;
    printf("Enter a character to send (press Ctrl+C to exit):\n");

    while (1) {
        printf("Input: ");
        scanf(" %c", &ch);  // Note the space before %c to skip whitespace/newlines

        if (write(uart0, &ch, 1) < 0) {
            perror("UART write failed");
        } else {
            printf("Sent: %c\n", ch);
        }
    }
  close(uart0);
    return 0;
}

