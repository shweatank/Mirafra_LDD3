#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define UART_DEVICE "/dev/serial0"
#define KERNEL_DEVICE "/dev/virt_uart_led"

int main() {
    int uart_fd, dev_fd;
    char buf[64];
    struct termios options;

    uart_fd = open(UART_DEVICE, O_RDONLY | O_NOCTTY);
    if (uart_fd < 0)
    {
        perror("Failed to open UART");
        return 1;
    }

    tcgetattr(uart_fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;
    tcsetattr(uart_fd, TCSANOW, &options);

    dev_fd = open(KERNEL_DEVICE, O_WRONLY);
    if (dev_fd < 0) {
        perror("Failed to open kernel device");
        close(uart_fd);
        return 1;
    }

    printf("Listening on UART and sending to /dev/virt_uart_led...\n");

    while (1)
    {
        ssize_t len = read(uart_fd, buf, sizeof(buf) - 1);
        if (len > 0) 
	{
            buf[len] = '\0';
            printf("Received: %s", buf);

            // Send to kernel
            write(dev_fd, buf, len);
        }
    }

    close(uart_fd);
    close(dev_fd);
    return 0;
}

