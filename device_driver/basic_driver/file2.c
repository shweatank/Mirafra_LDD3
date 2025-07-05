#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
	int fd;
	char read_buf[100];
	//const char *write_str="hello world";
	snprintf(read_buf,sizeof(read_buf),"%s %s %s",argv[1],argv[2],argv[3]);
	fd=open("/dev/simple_char_dev",O_RDWR);
	if(fd<0)
	{
		perror("failed to open device");
		return 1;

	}
	write(fd,read_buf,strlen(read_buf));
	//lseek(fd,0,SEEK_SET);
	//read(fd,read_buf,sizeof(read_buf));
	//printf("read from driver : %s\n",read_buf);
	//printf("String sent to kernel: %s\n",write_str);
	printf("sent:%s\n",read_buf);
	close(fd);
}
