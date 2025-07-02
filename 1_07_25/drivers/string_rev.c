#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
void main()
{
	int fd=open("/dev/simple_char_dev",O_RDWR);
	if(fd<0)
	{
		perror("fd error");
		return;
	}
	write(fd,"hello",5);
	lseek(fd,0,SEEK_SET);
	char buf[32];
	int n=read(fd,buf,5);
	buf[n]='\0';
	printf("Data read: %s\n",buf);
	close(fd);
}
