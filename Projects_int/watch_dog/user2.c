// user_bcm2711_wdt_reset.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define CMD_START 1
#define CMD_STOP  2
#define CMD_PING  3

int main() {
    int fd = open("/dev/bcm2711_wdt", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    printf("Starting WDT...\n");
    ioctl(fd, CMD_START);

    // Ping 5 times, every 5 seconds
    for (int i = 1; i <= 5; i++) {
        sleep(5);
        printf("Pinging WDT (%d/5)...\n", i);
        ioctl(fd, CMD_PING);
    }

    printf("Done pinging. WDT will now expire and reset the system.\n");

    // Close device without stopping WDT
    close(fd);

    // Wait indefinitely for WDT to reset
    while (1) {
        sleep(1);
    }

    return 0;
}
