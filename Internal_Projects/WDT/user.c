// test_wdt_ping_stop.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define WDT_MAGIC 'W'
#define WDT_START _IO(WDT_MAGIC, 0)
#define WDT_PING  _IO(WDT_MAGIC, 1)
#define WDT_STOP  _IO(WDT_MAGIC, 2)

int main() {
    int fd = open("/dev/generic_wdt", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Starting watchdog...\n");
    ioctl(fd, WDT_START);

    // Ping watchdog 3 times at 5-second intervals
    for (int i = 1; i <= 3; i++) {
        sleep(5);
        printf("Pinging watchdog (iteration %d)...\n", i);
        ioctl(fd, WDT_PING);
    }

    printf("Letting watchdog run without ping for 12 seconds...\n");
    sleep(12);  // Optional: allow timer to expire if you want to test timeout

    printf("Stopping watchdog...\n");
    ioctl(fd, WDT_STOP);

    close(fd);
    printf("Watchdog test finished.\n");
    return 0;
}
