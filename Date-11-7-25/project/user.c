#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define IOCTL_CLEAR _IO('k', 1)

void print_current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[USER-TIME %lld.%09ld]\n", (long long)ts.tv_sec, ts.tv_nsec);
}

void read_keys() {
    int fd = open("/dev/virtkeydev", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open /dev/virtkeydev");
        return;
    }

    char buf[1024];
    int len = read(fd, buf, sizeof(buf) - 1);
    if (len > 0) {
        buf[len] = '\0';
        print_current_time();
        printf("%s", buf);
    } else {
        perror("Read failed");
    }

    close(fd);
}

void clear_buffer() {
    int fd = open("/dev/virtkeydev", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/virtkeydev");
        return;
    }

    printf("Sending IOCTL_CLEAR...\n");
    if (ioctl(fd, IOCTL_CLEAR) < 0) {
        perror("IOCTL_CLEAR failed");
    } else {
        printf("Kernel buffer cleared.\n");
    }

    close(fd);
}

int main() {
    int choice;

    while (1) {
        printf("\nEnter Command:\n");
        printf("1) Clear buffer\n");
        printf("2) Read key log\n");
        printf("3) Exit\n> ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                clear_buffer();
                break;
            case 2:
                read_keys();
                break;
            case 3:
                printf("Exiting.\n");
                return 0;
            default:
                printf("Invalid option.\n");
        }
    }

    return 0;
}

