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
	write(fd,"good morning",12);
	printf("Data written\n");
	lseek(fd,0,SEEK_SET);
	char buf[32]={0};
	int n=read(fd,buf,5);
	buf[n]='\0';
	printf("Data read: %s\n",buf);
	close(fd);
}
