#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
void main()
{
	int fd;
	char read_data[100];
	const char *write_data="Hello from user space!";
	fd = open("/dev/simple_char_devv",O_RDWR);
	if(fd<0)
	{
		perror("Failed to open the file");
		return;
	}
	write(fd,write_data,strlen(write_data));
	lseek(fd,0,SEEK_SET);
	read(fd,read_data,sizeof(read_data));
	printf("Data from driver : %s\n",read_data);
	close(fd);
}
