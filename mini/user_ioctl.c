#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <string.h>
#include <errno.h>

#define DEVICE "/dev/virt_temp"

#define IOCTL_SET_THRESHOLD _IOW('t', 1, int)
#define IOCTL_GET_THRESHOLD _IOR('t', 2, int)
#define IOCTL_GET_ALERTS    _IOR('t', 3, int)
#define IOCTL_SET_TEMP      _IOW('t', 5, int)
#define IOCTL_SET_MANUAL    _IOW('t', 6, int)

void print_status(int fd) {
    int val;

    if (ioctl(fd, IOCTL_GET_THRESHOLD, &val) == 0)
        printf("Current threshold: %d\n", val);
    else
        perror("ioctl get threshold");

    if (ioctl(fd, IOCTL_GET_ALERTS, &val) == 0)
        printf("Alert count: %d\n", val);
    else
        perror("ioctl get alerts");
}

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Open device");
        return 1;
    }

    printf("Enabling manual mode...\n");
    int manual = 1;
    if (ioctl(fd, IOCTL_SET_MANUAL, &manual) < 0) {
        perror("ioctl set manual mode");
        close(fd);
        return 1;
    }

    int temp = 55;
    printf("Setting manual temperature to %d\n", temp);
    if (ioctl(fd, IOCTL_SET_TEMP, &temp) < 0) {
        perror("ioctl set temperature");
        close(fd);
        return 1;
    }

    print_status(fd);

    struct pollfd pfd = {
        .fd = fd,
        .events = POLLIN,
    };

    printf("Waiting for alerts (manual mode)... Press Ctrl+C to exit.\n");

    while (1) {
        int ret = poll(&pfd, 1, -1);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            perror("poll");
            break;
        }
        if (pfd.revents & POLLIN) {
            char buf[128];
            ssize_t n = read(fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                printf("Event: %s\n", buf);
            } else {
                perror("read");
            }
            print_status(fd);
        }
    }

    // Optionally disable manual mode before exit
    manual = 0;
    ioctl(fd, IOCTL_SET_MANUAL, &manual);

    close(fd);
    return 0;
}

