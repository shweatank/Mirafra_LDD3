//24 June 2025
//Program for file operations using system calls
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
void main()
{
	int fd=open("text1.txt",O_RDWR|O_CREAT,0664);
	char out;
	write(fd,"Mirafra",7);
	read(fd,&out,1);
	close(fd);
}
