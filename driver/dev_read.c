#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
void main()
{
	int fd=open("/dev/simple_char_dev",O_RDONLY);
	if(fd<0)
	{
		perror("fd error");
		return;
	}
	char buf[32];
	int n=read(fd,buf,2);
	buf[n]='\0';
	printf("Data read from the file is %s\n",buf);
	close(fd);
}
