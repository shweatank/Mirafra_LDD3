#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <gpiod.h>

#define BUF_SIZE 128
#define CHIP_NAME "gpiochip0"
#define GPIO_LINE 17
#define CONSUMER "led-blinker"

int main() {
    int keyfd = open("/dev/keylog", O_RDONLY);
    if (keyfd < 0) {
        perror("Failed to open /dev/keylog");
        return 1;
    }

    int uart0 = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0 < 0) {
        perror("UART open failed");
        close(keyfd);
        return 1;
    }

    struct termios options;
    tcgetattr(uart0, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag = 0;
    options.c_iflag = 0;
    options.c_oflag = 0;
    tcsetattr(uart0, TCSANOW, &options);

    struct gpiod_chip *chip = gpiod_chip_open_by_name(CHIP_NAME);
    struct gpiod_line *line = gpiod_chip_get_line(chip, GPIO_LINE);
    if (gpiod_line_request_output(line, CONSUMER, 0) < 0) {
        perror("GPIO request failed");
        return 1;
    }

    char buf[BUF_SIZE];
    char uart_in[BUF_SIZE];
    int uart_idx = 0;

    printf("Listening from /dev/keylog and UART...\n");

    while (1) {
        ssize_t n = read(keyfd, buf, sizeof(buf));
        if (n > 0) {
            write(uart0, buf, n);
            printf("Sent to UART: %.*s", (int)n, buf);
        }

        // Read UART response
        char ch;
        while (read(uart0, &ch, 1) > 0) {
            if (ch == '\n') {
                uart_in[uart_idx] = '\0';

                if (strcmp(uart_in, "ledon") == 0) {
                    gpiod_line_set_value(line, 1);
                    printf("-> LED ON\n");
                } else if (strcmp(uart_in, "ledoff") == 0) {
                    gpiod_line_set_value(line, 0);
                    printf("-> LED OFF\n");
                } else {
                    printf("-> Unknown UART command: %s\n", uart_in);
                }

                uart_idx = 0;
            } else {
                if (uart_idx < BUF_SIZE - 1)
                    uart_in[uart_idx++] = ch;
            }
        }

        usleep(100000);  // 100 ms
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    close(uart0);
    close(keyfd);
    return 0;
}

