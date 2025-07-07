// File: vmem_app.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "vmem_ioctl.h"

#define DEVICE_PATH "/dev/vmem"

void print_stats(int fd) {
    struct vmem_stats stats;
    if (ioctl(fd, IOCTL_GET_STATS, &stats) == -1) {
        perror("IOCTL_GET_STATS failed");
        return;
    }
    printf("Total Size: %zu\nBytes Read: %zu\nBytes Written: %zu\n",
           stats.total_size, stats.bytes_read, stats.bytes_written);
}

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Setting memory size to 4096 bytes...\n");
    size_t new_size = 4096;
    if (ioctl(fd, IOCTL_SET_MEM_SIZE, &new_size) == -1) {
        perror("IOCTL_SET_MEM_SIZE failed");
        close(fd);
        return 1;
    }

    const char *msg = "Hello, Virtual Memory Device!";
    printf("Writing message: '%s'\n", msg);
    if (write(fd, msg, strlen(msg)) == -1) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    lseek(fd, 0, SEEK_SET);
    char buf[100] = {0};
    printf("Reading back message...\n");
    if (read(fd, buf, sizeof(buf)) == -1) {
        perror("Read failed");
        close(fd);
        return 1;
    }
    printf("Read Message: '%s'\n", buf);

    print_stats(fd);

    printf("Clearing memory...\n");
    if (ioctl(fd, IOCTL_CLEAR_MEM) == -1) {
        perror("IOCTL_CLEAR_MEM failed");
    }

    print_stats(fd);

    close(fd);
    return 0;
}

