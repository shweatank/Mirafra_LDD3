#include<stdio.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<fcntl.h>

#define MAGIC 124
#define IOCTL_SET _IOW(MAGIC,'1',int)
#define IOCTL_GET _IOR(MAGIC,'2',int)
void main()
{
	int value,get_value;
	int fd=open("/dev/ioctl_gpio",O_RDWR);
	if(fd<0)
	{
		perror("open failed");
		return;
	}
	printf("Enter a value (0 for turnoff and 1 for turnon) ");
	scanf("%d",&value);
	ioctl(fd,IOCTL_SET,&value);
	ioctl(fd,IOCTL_GET,&get_value);
	printf("Status of value is %d\n",get_value);
	close(fd);
}
