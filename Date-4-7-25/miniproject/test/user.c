#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAJOR_NUM 100
#define IOCTL_SET_VALUE _IOW(MAJOR_NUM, 0, int)
#define IOCTL_GET_VALUE _IOR(MAJOR_NUM, 1, int)
#define DEVICE "/dev/ioctl_demo"

struct cal
{
	int a;
	int b;
	int flag;
}c;
int result=0;
int main() 
{
    int fd; 

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        perror("Failed to open device");
        return 1;
    }
    c.flag=0;
    printf("Enter The Send value to kernel using ioctl\n");
    scanf("%d%d",&c.a,&c.b);
    ioctl(fd, IOCTL_SET_VALUE, &c);

    while (1) 
    {
        sleep(1);
        ioctl(fd, IOCTL_GET_VALUE, &result);
        if (result != 0 || c.flag == 1)
            break;
        printf("Waiting for kernel operation...\n");
    }
    //printf("Requesting The result From The Kernel\n");
    //ioctl(fd, IOCTL_GET_VALUE, &result);

    printf("Received value from kernel: %d\n", result);
    close(fd);
    return 0;
}

