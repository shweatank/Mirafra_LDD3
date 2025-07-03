#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define IOCTL_MAGIC 0xF0
#define IOCTL_SEND_MODE _IOW(IOCTL_MAGIC,1,device_mode_t)
#define IOCTL_GET_MODE _IOR(IOCTL_MAGIC,2,device_mode_t)

typedef enum
{
	MODE_OFF = 0,MODE_ON,MODE_ERROR
}device_mode_t;

int main() {
    int fd = open("/dev/sample", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    device_mode_t mode_send = MODE_ON;
    device_mode_t mode_recv;

    // Send mode to driver
    if (ioctl(fd, IOCTL_SEND_MODE, &mode_send) < 0) {
        perror("ioctl send failed");
        close(fd);
        return 1;
    }

    // Get mode from driver
    if (ioctl(fd, IOCTL_GET_MODE, &mode_recv) < 0) {
        perror("ioctl get failed");
        close(fd);
        return 1;
    }

    printf("Received mode from driver: %d\n", mode_recv);

    close(fd);
    return 0;
}

