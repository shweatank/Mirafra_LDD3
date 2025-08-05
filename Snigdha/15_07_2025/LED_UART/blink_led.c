#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define CHIP_NAME "gpiochip0"    // GPIO controller
#define GPIO_LINE 17             // BCM GPIO17 for LED
#define CONSUMER "uart-led"      // Identifier
#define UART_DEV "/dev/serial0"  // UART device

int setup_uart(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("UART open failed");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Raw input
    options.c_iflag &= ~(IXON | IXOFF | IXANY);          // No flow control
    options.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &options);
    return fd;
}

int main() {
    struct gpiod_chip *chip = gpiod_chip_open_by_name(CHIP_NAME);
    if (!chip) {
        perror("Failed to open GPIO chip");
        return 1;
    }

    struct gpiod_line *line = gpiod_chip_get_line(chip, GPIO_LINE);
    if (!line) {
        perror("Failed to get GPIO line");
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_request_output(line, CONSUMER, 0) < 0) {
        perror("Failed to request GPIO line as output");
        gpiod_chip_close(chip);
        return 1;
    }

    int uart_fd = setup_uart(UART_DEV);
    if (uart_fd < 0) {
        gpiod_line_release(line);
        gpiod_chip_close(chip);
        return 1;
    }

    printf("Ready: Send 'a' for LED ON, 'b' for OFF. Echo enabled.\n");

    char c;
    while (1) {
        int n = read(uart_fd, &c, 1);
        if (n > 0) {
            // Echo the character back to UART
            write(uart_fd, &c, 1);

            // LED control
            if (c == 'a') {
                gpiod_line_set_value(line, 1);
                printf("LED ON\n");
            } else if (c == 'b') {
                gpiod_line_set_value(line, 0);
                printf("LED OFF\n");
            } else {
                printf("Unknown input: %c\n", c);
            }
        } else {
            usleep(100000);  // Sleep briefly to avoid busy loop
        }
    }

    // Cleanup (not reachable in infinite loop)
    gpiod_line_release(line);
    gpiod_chip_close(chip);
    close(uart_fd);
    return 0;
}

