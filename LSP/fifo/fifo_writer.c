#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
int main()
{
	if(mkfifo("myfifo",0666)==-1)
	{
		perror("mkfifo");
		return EXIT_FAILURE;
	}
	int fd;
	fd=open("myfifo",O_WRONLY);
	char buf[]="hello from writer";
	int len=strlen(buf)+1;
	if(write(fd,buf,len)==len)
		printf("data written successfully\n");
	else
		printf("failed to write data\n");
	close(fd);
}
