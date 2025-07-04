#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "int_calc.h"

int main() {
    int fd = open("/dev/int_calc", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Enter two numbers and an operator (e.g., 12+5), then press Enter\n");
    ioctl(fd, IOCTL_START_INPUT);

    sleep(10); // wait for user input via keyboard

    ioctl(fd, IOCTL_STOP_INPUT);

    int result;
    ioctl(fd, IOCTL_GET_RESULT, &result);

    printf("Result: %d\n", result);

    close(fd);
    return 0;
}

