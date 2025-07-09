#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
int main()
{
	int fd;
	char buf[20],temp[20];
	printf("entre the string\n");
	gets(buf);
	fd=open("data1.txt",O_RDWR,0664);
	if(fd==-1)
	{
		perror("mesaage");
		exit(0);
	}
	write(fd,buf,strlen(buf));
        printf("write operation completed\n");
	read(fd,temp,20);
	printf("read operation completed\n");
	puts(temp);
	close(fd);
}

