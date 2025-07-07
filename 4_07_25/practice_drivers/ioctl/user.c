#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>

#define MAJOR_NUM 121
#define WR_VALUE _IOW(MAJOR_NUM,0,int)
#define RD_VALUE _IOR(MAJOR_NUM,1,int)

void main()
{
	int num=10;
	int result=0;
	int fd=open("/dev/ioctl_demo",O_RDWR);
	if(fd<0)
	{
		perror("open error");
		return;
	}
	ioctl(fd,WR_VALUE,&num);
	lseek(fd,0,SEEK_SET);
	ioctl(fd,RD_VALUE,&result);
	printf("%d\n",result);
	close(fd);
}
