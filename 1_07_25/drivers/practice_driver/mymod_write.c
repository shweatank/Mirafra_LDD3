#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
void main()
{
	int fd=open("/dev/mymod",O_RDWR);
	if(fd<0)
	{
		perror("fd error");
		return;
	}
	int n=30;
	write(fd,&n,4);
	lseek(fd,0,SEEK_SET);
	int num;
	read(fd,&num,4);
	printf("%d\n",num);
	close(fd);
}
