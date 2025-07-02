#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
void main()
{
	int fd=open("/dev/mymod1",O_RDWR);
	if(fd<0)
	{
		perror("fd error");
		return;
	}
	write(fd,"Hello",5);
	char buf[32];
	int n=read(fd,buf,5);
	buf[n]='\0';
	printf("Data read %s\n",buf);
	close(fd);
}
