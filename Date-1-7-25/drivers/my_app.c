#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
	int fd;
	char read_buf[300];
	const char *write_str="hello from user space";

	fd=open("/dev/simple_char_dev",O_RDWR);

	write(fd,write_str,strlen(write_str));
	lseek(fd,0,SEEK_SET);
	read(fd,read_buf,sizeof(read_buf));
	printf("Read from driver:%s\n",read_buf);
	close(fd);
}

