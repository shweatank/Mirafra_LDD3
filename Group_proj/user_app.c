#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define IOCTL_GET_LOG_SIZE _IOR('e', 1, int)

int main() {
    int fd = open("/dev/event_logger", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    int size = 0;
    if (ioctl(fd, IOCTL_GET_LOG_SIZE, &size) == -1) {
        perror("ioctl");
        return 1;
    }

    char buffer[1024];
    ssize_t bytes = read(fd, buffer, size);
    if (bytes < 0) {
        perror("read");
        return 1;
    }

    buffer[bytes] = '\0';
    printf("Logs:\n%s\n", buffer);
    close(fd);
    return 0;
}

