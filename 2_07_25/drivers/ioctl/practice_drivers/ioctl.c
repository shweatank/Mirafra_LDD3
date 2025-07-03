#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<sys/ioctl.h>
#define MAJOR_NUM 111
#define WR_VALUE _IOW(MAJOR_NUM,0,int)
#define RD_VALUE _IOR(MAJOR_NUM,1,int)

void main()
{
	int val=20;
	int read_val=0;
	int fd;
	fd=open("/dev/mymod_ioctl",O_RDWR);
	if(fd<0)
	{
		perror("open error");
		return;
	}
	ioctl(fd,WR_VALUE,&val);
	ioctl(fd,RD_VALUE,&read_val);
	printf("Sent number is %d\n",val);
	printf("Received number is %d\n",read_val);
	close(fd);
}
