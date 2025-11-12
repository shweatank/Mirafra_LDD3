#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>

int fd;

void stop_watchdog(int sig) {
    printf("\nSignal received. Stopping watchdog...\n");
    write(fd, "V", 1);  // Magic character to disable watchdog
    close(fd);
    exit(0);
}

int main() {
    fd = open("/dev/watchdog", O_WRONLY);
    if (fd < 0) {
        perror("Cannot open /dev/watchdog");
        return 1;
    }

    // Setup signal handler
    signal(SIGINT, stop_watchdog);  // Ctrl+C
    signal(SIGTERM, stop_watchdog); // kill <pid>

    printf("Watchdog started. Will ping every 5 seconds.\n");
    printf("Press Ctrl+C or send SIGTERM to stop the watchdog.\n");

    while (1) {
        ioctl(fd, WDIOC_KEEPALIVE, 0);
        printf("Watchdog pinged.\n");
        sleep(5);
    }

    return 0;
}
