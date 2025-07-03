#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE "/dev/ioctl_demo"
#define MAJOR_NUM 100

// Correct macro definitions
#define IOCTL_SET_NUM _IOW(MAJOR_NUM, 0, int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM, 1, int)

int main()
{
    int fd;
    int val = 1234;
    int read_val = 0;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open device");
        return 1;
    }

    printf("Sending value %d to kernel...\n", val);
    if (ioctl(fd, IOCTL_SET_NUM, &val) < 0)
    {
        perror("IOCTL_SET_NUM failed");
        close(fd);
        return 1;
    }

    printf("Requesting value from kernel...\n");
    if (ioctl(fd, IOCTL_GET_NUM, &read_val) < 0)
    {
        perror("IOCTL_GET_NUM failed");
        close(fd);
        return 1;
    }

    printf("Received value from kernel: %d\n", read_val);

    close(fd);
    return 0;
}

