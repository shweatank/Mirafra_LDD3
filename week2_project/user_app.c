#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "traffic_light_ioctl.h"

void show_menu() {
    printf("\nTraffic Light Control Menu:\n");
    printf("1. Set RED\n2. Set GREEN\n3. Set YELLOW\n");
    printf("4. Enable IRQ cycling\n5. Disable IRQ cycling\n6. Read current state\n7. Exit\n");
    printf("Enter choice: ");
}

int main() {
    int fd = open("/dev/traffic_light_irq", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    int choice;
    char buf[32];

    while (1) {
        show_menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1: ioctl(fd, TL_SET_RED); break;
            case 2: ioctl(fd, TL_SET_GREEN); break;
            case 3: ioctl(fd, TL_SET_YELLOW); break;
            case 4: ioctl(fd, TL_IRQ_ENABLE); break;
            case 5: ioctl(fd, TL_IRQ_DISABLE); break;
            case 6:
                lseek(fd, 0, SEEK_SET);
                read(fd, buf, sizeof(buf));
                printf("Current Light: %s", buf);
                break;
            case 7:
                close(fd);
                return 0;
        }
    }
}

