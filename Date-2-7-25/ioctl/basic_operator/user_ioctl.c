#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAJOR_NUM 100
#define IOCTL_SET_VALUE _IOW(MAJOR_NUM, 0, int)
#define IOCTL_GET_VALUE _IOR(MAJOR_NUM, 1, int)
#define DEVICE "/dev/ioctl_demo"
int main() 
{
    int fd, value=1234,read=0;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        perror("Failed to open device");
        return 1;
    }

    printf("Sending value: %d to kernel using ioctl\n", value);
    ioctl(fd, IOCTL_SET_VALUE, &value);
    printf("Requesting The Value From The Kernel\n");
    ioctl(fd, IOCTL_GET_VALUE, &read);

    printf("Received value from kernel: %d\n", read);
    close(fd);
    return 0;
}

