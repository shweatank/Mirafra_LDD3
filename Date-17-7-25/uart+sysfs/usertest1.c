#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define SYSFS_TX "/sys/kernel/my_sysfs/tx_data"
#define SYSFS_RX "/sys/kernel/my_sysfs/rx_data"
#define UART_DEV "/dev/ttyUSB0"

int main() {
    int uart_fd = open(UART_DEV, O_RDWR | O_NOCTTY);
    if (uart_fd < 0) {
        perror("UART open failed");
        return 1;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);
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
    tcsetattr(uart_fd, TCSANOW, &options);

    char tx_buf[100];
    char rx_buf[100];

    while (1) {
        // 1. Read from sysfs tx_data
        int tx_fd = open(SYSFS_TX, O_RDONLY);
        if (tx_fd < 0) {
            perror("Open sysfs tx_data");
            break;
        }

        ssize_t tx_len = read(tx_fd, tx_buf, sizeof(tx_buf) - 1);
        close(tx_fd);

        if (tx_len > 0) {
            tx_buf[tx_len] = '\0';
            printf("[TX] Sysfs → UART: %s\n", tx_buf);

            // 2. Send via UART
            write(uart_fd, tx_buf, strlen(tx_buf));
        }

        // 3. Read response from UART
        ssize_t rx_len = read(uart_fd, rx_buf, sizeof(rx_buf) - 1);
        if (rx_len > 0) {
            rx_buf[rx_len] = '\0';
            printf("[RX] UART → Sysfs: %s\n", rx_buf);

            // 4. Write to sysfs rx_data
            int rx_fd = open(SYSFS_RX, O_WRONLY);
            if (rx_fd < 0) {
                perror("Open sysfs rx_data");
                break;
            }
            write(rx_fd, rx_buf, rx_len);
            close(rx_fd);
        }

        sleep(1); // avoid busy loop
    }

    close(uart_fd);
    return 0;
}

