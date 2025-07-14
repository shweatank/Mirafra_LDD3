#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define KEYLOG_IOCTL_GET_COUNT   _IOR('K', 1, int)
#define KEYLOG_IOCTL_RESET_COUNT _IO('K', 2)
#define KEYLOG_IOCTL_ENABLE      _IO('K', 3)
#define KEYLOG_IOCTL_DISABLE     _IO('K', 4)

int main() {
    int fd, choice, count;

    fd = open("/dev/keylogger", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/keylogger");
        return 1;
    }

    while (1) {
        printf("\n--- Keylogger Menu ---\n");
        printf("1. Get keypress count\n");
        printf("2. Reset count\n");
        printf("3. Enable logging\n");
        printf("4. Disable logging\n");
        printf("5. Simulate keypress (open/close)\n");
        printf("6. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (ioctl(fd, KEYLOG_IOCTL_GET_COUNT, &count) == 0)
                    printf("Total keypresses (open calls): %d\n", count);
                else
                    perror("IOCTL GET_COUNT failed");
                break;
            case 2:
                ioctl(fd, KEYLOG_IOCTL_RESET_COUNT);
                printf("Counter reset.\n");
                break;
            case 3:
                ioctl(fd, KEYLOG_IOCTL_ENABLE);
                printf("Logging enabled.\n");
                break;
            case 4:
                ioctl(fd, KEYLOG_IOCTL_DISABLE);
                printf("Logging disabled.\n");
                break;
            case 5: {
                int sim = open("/dev/keylogger", O_RDONLY);
                if (sim >= 0) close(sim);
                printf("Simulated keypress via open().\n");
                break;
            }
            case 6:
                close(fd);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    close(fd);
    return 0;
}

