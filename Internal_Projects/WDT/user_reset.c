#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>

int main() {
    int fd = open("/dev/watchdog", O_RDWR);
    if (fd < 0) {
        perror("Cannot open /dev/watchdog");
        return 1;
    }

    printf("Watchdog started (timeout 10s)\n");

    int timeout = 10;
    ioctl(fd, WDIOC_SETTIMEOUT, &timeout);

    printf("System will reset in ~10 seconds if not fed...\n");
    sleep(30);  // wait longer than timeout

    // Do NOT feed or stop the watchdog
    // close(fd);  // leaving device open ensures reset occurs

    return 0;
}

