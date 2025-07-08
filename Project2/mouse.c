#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "mouse_ioctl.h"

int main(void)
{
    int fd = open("/dev/mouse_clicks", O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    while (1) {
        unsigned long clicks = 0;

        if (ioctl(fd, IOCTL_GET_COUNT, &clicks) == 0)
            printf("\rClicks: %lu   ", clicks);
        else
            perror("ioctl");

        fflush(stdout);
        usleep(200 * 1000);   // 200 ms

        if (clicks >= 20) {   // arbitrary demo threshold
            puts("\nresetting counter …");
            ioctl(fd, IOCTL_CLR_COUNT);
        }
    }
    return 0;
}

