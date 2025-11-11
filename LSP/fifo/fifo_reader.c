#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
int main()
{
	/*if(mkfifo("myfifo",0666)==-1)
	{
		perror("mkfifo");
		return EXIT_FAILURE;
	}*/
	int fd;
	fd=open("myfifo",O_RDONLY);
	char buf[50];
	if(read(fd,buf,sizeof(buf))>0)
		printf("Data recieved:%s\n",buf);
	close(fd);
}
