#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
void main()
{
	int fd,numin=100,numout;
	//char read_data[100];
	const char *write_data="Hello from user space!";
	fd = open("/dev/simple_char_dev_intt",O_RDWR);
	if(fd<0)
	{
		perror("Failed to open the file");
		return;
	}
	write(fd,&numin,4);
	lseek(fd,0,SEEK_SET);
	read(fd,&numout,4);
	printf("Data from driver : %d\n",numout);
	close(fd);
}
