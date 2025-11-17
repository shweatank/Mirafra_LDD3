#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>

volatile int stop = 0;

void handle_sigint(int sig) {
    stop = 1; // allow clean exit if Ctrl+C pressed
}

int main() {
    int fd = open("/dev/watchdog", O_RDWR);
    if (fd < 0) {
        perror("Cannot open /dev/watchdog");
        return 1;
    }

    signal(SIGINT, handle_sigint);

    int timeout = 10; // seconds
    if (ioctl(fd, WDIOC_SETTIMEOUT, &timeout) < 0) {
        perror("Failed to set watchdog timeout");
    }

    printf("Watchdog started with timeout %d seconds\n", timeout);
    printf("Simulating pretimeout warning...\n");

    // Countdown loop (pretend pretimeout)
    for (int t = timeout; t > 0 && !stop; t--) {
        printf("WARNING: Watchdog will reset in ~%d seconds if not fed\n", t);
        sleep(1);
    }

    // After countdown, stop feeding to trigger WDT reset
    printf("\nPing failed! Not feeding watchdog anymore. System will reset...\n");

    // Infinite loop: do not feed the watchdog
    while (!stop) {
        sleep(1); // wait for hardware WDT to reset the system
    }

    // Optional: stop watchdog safely if program interrupted before reset
    // write(fd, "V", 1);

    close(fd);
    return 0;
}

