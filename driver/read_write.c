#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
void main()
{
	int fd=open("/dev/simple_char_dev",O_RDWR);
	if(fd<0)
	{
		perror("fd error");
		return;
	}
	write(fd,"heyy Hii",7);
	printf("Data written\n");
	lseek(fd,0,SEEK_SET);
	char buf[32];
	int n=read(fd,buf,7);
	buf[n]='\0';
	printf("Data read: %s\n",buf);
	close(fd);
}
