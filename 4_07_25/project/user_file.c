#include<unistd.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<fcntl.h>

#define MAJOR 111
#define IOCTL_SET_NUM _IOW(MAJOR,1,int)
#define IOCTL_GET_NUM _IOR(MAJOR,2,int)

void main()
{
	int fd=open("/dev/volume_control",O_RDWR);
	if(fd<0)
	{
		perror("open error");
		return;
	}
	int volume=75;
	if(ioctl(fd,IOCTL_SET_NUM,&volume)==-1)
	{
		perror("IOCTL_SET_NUM");
		return;
	}
	char inc='+';
	char dec='-';
	write(fd,&inc,1);
	write(fd,&inc,1);
	if(ioctl(fd,IOCTL_GET_NUM,&volume)==-1)
	{
		perror("IOCTL_GET_NUM");
		return;
	}
	printf("Volume is %d\n",volume);
	close(fd);
}
