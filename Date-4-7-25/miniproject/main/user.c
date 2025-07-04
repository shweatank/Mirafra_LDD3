#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/ioctl_demo"
#define MAJOR_NUM 100
#define IOCTL_SEND_STRING _IOW(MAJOR_NUM, 2, char *)
#define IOCTL_GET_STRING _IOR(MAJOR_NUM, 3, char *)

int main()
{
    int fd = open(DEVICE, O_RDWR);

    char str[256];
    printf("Enter The String:\n");
    scanf(" %[^\n]",str);
    ioctl(fd, IOCTL_SEND_STRING, str);
    char result[256] = {0};
    while (1)
    {
        memset(result, 0, sizeof(result));
        ioctl(fd, IOCTL_GET_STRING, result);
	printf("Waiting for kernel>>>\n");
        if (strlen(result) > 0) 
	{
            printf("Result from kernel: %s\n", result);
            break;  
        }

        sleep(1); 
    }
    close(fd);
    return 0;
}

