#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
void main()
{
	int fd=open("/dev/simple_char_dev",O_WRONLY);
	if(fd<0)
	{
		perror("fd error");
		return;
	}
	write(fd,"Good afternoon",14);
	printf("Data written into file\n");
       	close(fd);
}
