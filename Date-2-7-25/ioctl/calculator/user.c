#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAJOR_NUM 100
#define IOCTL_SET_VALUE _IOW(MAJOR_NUM, 0, int)
#define IOCTL_GET_VALUE _IOR(MAJOR_NUM, 1, int)
#define DEVICE "/dev/ioctl_demo"
struct calc_packet
{
    int a;
    int b;
    char op;
};

int main() 
{
    int fd, result=0;
    c.a=100;
    c.b=20;
    c.op='/';
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        perror("Failed to open device");
        return 1;
    }
    //printf("Sending value: %d to kernel using ioctl\n", value);
    ioctl(fd, IOCTL_SET_VALUE, &c);
    printf("Requesting The Value From The Kernel\n");
    ioctl(fd, IOCTL_GET_VALUE, &result);

    printf("Received value from kernel: %d\n", result);
    close(fd);
    return 0;
}

