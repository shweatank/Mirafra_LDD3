#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define UART_DEVICE "/dev/ttyUSB0"

int main() {
    printf("before open...\n");
    int uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
    printf("after open...\n");
    if (uart_fd < 0) {
        perror("Failed to open UART");
        return 1;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    tcsetattr(uart_fd, TCSANOW, &options);

    char input;
    printf("Send 'a' to turn LED ON, 'b' to turn it OFF. Press 'q' to quit.\n");

    while (1) {
        printf("Enter command: ");
        scanf(" %c", &input);  // Space before %c to skip any leftover newline

        if (input == 'q') {
            break;
        }

        if (input == 'a' || input == 'b') {
            write(uart_fd, &input, 1);
            printf("Sent '%c'\n", input);
        } else {
            printf("Invalid input. Use 'a', 'b', or 'q'.\n");
        }
    }

    close(uart_fd);
    return 0;
}

