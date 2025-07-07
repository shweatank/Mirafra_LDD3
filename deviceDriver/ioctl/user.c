#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/ioctl.h>

#define MAJOR_NUM 100
#define IOCTL_SET_NUM _IOW(MAJOR_NUM, 0, int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM, 1, int)
#define DEVICE "/dev/ioctl_demo"


int main()
{
    int fd;
    int val = 1234;
    int readVal = 0;	    

    fd = open(DEVICE, O_RDWR);
    if(fd < 0)
    {
        perror("Failed to open device");
	return 1;
    }

    printf("Sending value %d to kernel...\n", val);
    ioctl(fd, IOCTL_SET_NUM, &val);
    
    printf("Requesting value %d from kernel...\n", val);
    ioctl(fd, IOCTL_GET_NUM, &readVal);
    
    printf("Received value from kernel: %d\n", readVal);
    close(fd);

    return 0;
}
