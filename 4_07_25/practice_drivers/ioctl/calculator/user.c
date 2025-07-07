#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>

#define MAJOR 150
#define WR_VALUE1 _IOW(MAJOR,0,int)
#define WR_VALUE2 _IOW(MAJOR,1,int)
#define RD_VALUE _IOR(MAJOR,2,int)

void main()
{
	int value1,value2,result;
	printf("enter two numbers ");
	scanf("%d %d",&value1,&value2);
	int fd=open("/dev/ioctl_calculator",O_RDWR);
	if(fd<0)
	{
		perror("open error");
		return;
	}
	ioctl(fd,WR_VALUE1,&value1);
	ioctl(fd,WR_VALUE2,&value2);
	lseek(fd,0,SEEK_SET);
	ioctl(fd,RD_VALUE,&result);
	printf("%d\n",result);
	close(fd);
}
