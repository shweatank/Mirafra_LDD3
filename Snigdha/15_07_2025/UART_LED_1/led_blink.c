#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <wiringPi.h>

#define LED_PIN 0  // WiringPi pin 0 = BCM GPIO17

int setup_uart(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("UART open failed");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);

    // Set baud rate
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    // 8N1 config
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &options);
    return fd;
}

int main() {
    // Initialize wiringPi and GPIO
    if (wiringPiSetup() == -1) {
        printf("Failed to initialize wiringPi\n");
        return 1;
    }
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Open UART
    int uart_fd = setup_uart("/dev/ttyS0");
    if (uart_fd < 0) return 1;

    printf("UART LED control started. Send '1' or '0' from minicom\n");

    char ch;
    while (1) {
        int n = read(uart_fd, &ch, 1);
        if (n > 0) {
            if (ch == '1') {
                digitalWrite(LED_PIN, HIGH);
                printf("LED ON\n");
            } else if (ch == '0') {
                digitalWrite(LED_PIN, LOW);
                printf("LED OFF\n");
            } else {
                printf("Invalid input: %c\n", ch);
            }
        } else {
            usleep(100000); // Sleep 100ms if no data
        }
    }

    close(uart_fd);
    return 0;
}

