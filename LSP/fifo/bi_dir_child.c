#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
int main()
{
	char buf[]="hello from child",buff[50];
	int len=strlen(buf)+1;
/*	if(mkfifo("fifo1",0666)==-1)
	{
		perror("mkfifo1");
		return EXIT_FAILURE;
	}
	if(mkfifo("fifo2",0666)==-1)
	{
		perror("mkfifo2");
		return EXIT_FAILURE;
	}*/
	int fd1,fd2;
	fd1=open("fifo1",O_RDONLY);
	fd2=open("fifo2",O_WRONLY);
	if(read(fd1,buff,sizeof(buff))>0)
		printf("Child:recieved data:%s\n",buff);

	if((write(fd2,buf,len))==len)
		printf("Child:Data written successfully\n");
	else
		printf("Child:failed to write data\n");

}

