#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define UART0 "/dev/serial0"
#define UART1 "/dev/ttyAMA0"

#define GPIO_PIN "17"

// ---- GPIO LED (sysfs) ----
void gpio_export() {
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd >= 0) {
        write(fd, GPIO_PIN, strlen(GPIO_PIN));
        close(fd);
    }
}
void gpio_unexport() {
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd >= 0) {
        write(fd, GPIO_PIN, strlen(GPIO_PIN));
        close(fd);
    }
}
void gpio_direction_out() {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", GPIO_PIN);
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, "out", 3);
        close(fd);
    }
}
void gpio_set_value(int val) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", GPIO_PIN);
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, val ? "1" : "0", 1);
        close(fd);
    }
}
void led_on_for_2s() {
    gpio_set_value(1);
    sleep(2);
    gpio_set_value(0);
}

// ---- UART CONFIG ----
int setup_uart(const char *dev) {
    int fd = open(dev, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);

    cfsetospeed(&options, B115200);
    cfsetispeed(&options, B115200);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

// ---- MAIN ----
int main() {
    gpio_export(); sleep(1);
    gpio_direction_out();
    gpio_set_value(0);

    int uart0_fd = setup_uart(UART0);
    int uart1_fd = setup_uart(UART1);
    if (uart0_fd < 0 || uart1_fd < 0) {
        fprintf(stderr, "UART open failed\n");
        return 1;
    }

    while (1) {
        char c;
        printf("Enter a character: ");
        scanf(" %c", &c);

        // Send from UART0 → UART1
        write(uart0_fd, &c, 1);

        char recv;
        read(uart1_fd, &recv, 1);

        // UART1 increments and sends back
        char inc = recv + 1;
        write(uart1_fd, &inc, 1);

        char back;
        read(uart0_fd, &back, 1);
        printf("UART0 received: %c\n", back);

        if (back == c + 1) {
            printf("Match! LED ON\n");
            led_on_for_2s();
        } else {
            printf("No match. LED OFF\n");
        }
    }

    close(uart0_fd);
    close(uart1_fd);
    gpio_unexport();
    return 0;
}

