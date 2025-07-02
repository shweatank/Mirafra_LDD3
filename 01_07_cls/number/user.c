#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
void main()
{
	int fd;
	char read_data[100];
	//const char *write_data="Hello";
	int num=5;
	fd = open("/dev/simple_char_devv",O_RDWR);
	if(fd<0)
	{
		perror("Failed to open the file");
		return;
	}
	write(fd,&num,sizeof(num));
	lseek(fd,0,SEEK_SET);
	read(fd,&num,sizeof(num));
	printf("Data from driver : %s\n",num);;
	close(fd);
}
