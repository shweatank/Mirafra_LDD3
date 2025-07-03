#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "ioctl_struct.h"
#define DEVICE_NAME "/dev/ioctl_stuct"
int main() {
    int fd;
    struct my_data data;

    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Device open failed");
        return 1;
    }

    data.val1 = 100;
    data.val2 = 200;

    ioctl(fd, IOCTL_WRITE_DATA, &data);

    data.val1 = 0;
    data.val2 = 0;
    ioctl(fd, IOCTL_READ_DATA, &data);

    printf("From kernel: val1 = %d, val2 = %d\n", data.val1, data.val2);

    close(fd);
    return 0;
}

