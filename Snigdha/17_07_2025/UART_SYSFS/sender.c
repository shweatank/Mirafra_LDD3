// Compile with: gcc uart_sender.c -o sender
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int main() {
    int uart_fd = open("/dev/ttyUSB0", O_WRONLY | O_NOCTTY);
    if (uart_fd < 0) {
        perror("Failed to open UART");
        return 1;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(uart_fd, TCSANOW, &options);

    char input;
    printf("Enter 'a' to turn ON LED or 'b' to turn OFF LED: ");
    scanf(" %c", &input);  // Note the space to consume newline

    if (input == 'a' || input == 'b') {
        write(uart_fd, &input, 1);
        printf("Sent '%c' to Raspberry Pi\n", input);
    } else {
        printf("Invalid input\n");
    }

    close(uart_fd);
    return 0;
}

