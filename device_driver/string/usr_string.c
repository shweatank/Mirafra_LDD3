#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
int main()
{
	int fd;
	char read_buf[100];
	const char *write_str="hello world";
	fd=open("/dev/simple_string_dev",O_RDWR);
	if(fd<0)
	{
		perror("failed to open device");
		return 1;

	}
	write(fd, write_str, strlen(write_str));
	lseek(fd,0,SEEK_SET);
	read(fd,read_buf,sizeof(read_buf));
	printf("read from driver : %s\n",read_buf);
	/*printf("String sent to kernel: %s\n",write_str);
	read(fd,read_buf,sizeof(read_buf));
	printf("read from driver : %s\n",read_buf);*/
	close(fd);
}
