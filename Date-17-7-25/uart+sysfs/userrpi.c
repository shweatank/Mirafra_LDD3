// rx_uart_sysfs.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define SYSFS_RX "/sys/kernel/my_sysfs/rx_data"
#define UART_DEV "/dev/serial0"  // or ttyAMA0 on Pi

int main() {
    int fd_uart = open(UART_DEV, O_RDONLY | O_NOCTTY);
    if (fd_uart < 0) {
        perror("open UART");
        return 1;
    }

    struct termios options;
    tcgetattr(fd_uart, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd_uart, TCSANOW, &options);

    char buf[100];
    ssize_t n = read(fd_uart, buf, sizeof(buf)-1);
    if (n < 0) {
        perror("read UART");
        close(fd_uart);
        return 1;
    }
    buf[n] = '\0';  // null-terminate for safety
    close(fd_uart);

    int fd_sys = open(SYSFS_RX, O_WRONLY);
    if (fd_sys < 0) {
        perror("open sysfs rx");
        return 1;
    }

    write(fd_sys, buf, strlen(buf));
    close(fd_sys);

    return 0;
}

