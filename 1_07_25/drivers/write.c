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
	int num=30;
	write(fd,(char*)&num,sizeof(num));
	lseek(fd,0,SEEK_SET);
	int n=0;
	read(fd,(int *)&n,4);
//	n++;
	printf("Data read: %d\n",n);
	close(fd);
}
