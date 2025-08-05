#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = open("/dev/poll_dev", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;

    printf("Waiting for data using poll...\n");

    int ret = poll(&pfd, 1, 10000); // 10 sec timeout
    if (ret == -1) {
        perror("poll");
    } else if (ret == 0) {
        printf("Timeout! No data available.\n");
    } else {
        if (pfd.revents & POLLIN) {
            char buf[128] = {0};
            read(fd, buf, sizeof(buf));
            printf("Received: %s\n", buf);
        }
    }

    close(fd);
    return 0;
}
