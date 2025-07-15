#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>

#define SYSFS_THRESHOLD "/sys/kernel/virt_temp/threshold"
#define PROC_STATUS "/proc/virt_temp_status"
#define DEVICE "/dev/virt_temp"

void read_threshold() {
    char buf[32];
    FILE *f = fopen(SYSFS_THRESHOLD, "r");
    if (!f) {
        perror("open threshold");
        return;
    }
    if (fgets(buf, sizeof(buf), f))
        printf("Threshold (sysfs): %s", buf);
    fclose(f);
}

void write_threshold(int val) {
    FILE *f = fopen(SYSFS_THRESHOLD, "w");
    if (!f) {
        perror("open threshold for write");
        return;
    }
    fprintf(f, "%d\n", val);
    fclose(f);
    printf("Threshold set to %d via sysfs\n", val);
}

void read_status() {
    char buf[256];
    FILE *f = fopen(PROC_STATUS, "r");
    if (!f) {
        perror("open proc status");
        return;
    }
    printf("Current sensor status:\n");
    while (fgets(buf, sizeof(buf), f))
        printf("  %s", buf);
    fclose(f);
}

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Open device");
        return 1;
    }

    read_threshold();

    write_threshold(50);

    read_threshold();

    read_status();

    struct pollfd pfd = {
        .fd = fd,
        .events = POLLIN,
    };

    printf("Waiting for alert events (poll on /dev/virt_temp)...\n");

    while (1) {
        int ret = poll(&pfd, 1, -1);
        if (ret < 0) {
            perror("poll");
            break;
        }
        if (pfd.revents & POLLIN) {
            char buf[64];
            ssize_t n = read(fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = 0;
                printf("Event from driver: %s", buf);
            }
            read_status();
        }
    }
}
