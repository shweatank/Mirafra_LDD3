// tx_uart_sysfs.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define SYSFS_TX "/sys/kernel/my_sysfs/my_data"
#define UART_DEV "/dev/ttyUSB0"  // or ttyAMA0 on Pi

int main() {
    char buf[100];
    int fd_sys = open(SYSFS_TX, O_RDONLY);
    if (fd_sys < 0) {
        perror("open sysfs tx");
        return 1;
    }

    ssize_t n = read(fd_sys, buf, sizeof(buf)-1);
    if (n < 0) {
        perror("read sysfs");
        close(fd_sys);
        return 1;
    }
    buf[n] = '\0'; // null-terminate if needed
    close(fd_sys);

    int fd_uart = open(UART_DEV, O_WRONLY | O_NOCTTY);
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

    write(fd_uart, buf, strlen(buf));
    close(fd_uart);
    return 0;
}

