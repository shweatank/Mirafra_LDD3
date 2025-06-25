#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
struct fun
{
	int (*fp)();
	int (*fp1)();
	int (*fp2)();
	int (*fp3)();
};
struct fun s={
	.fp=open,
	.fp1=read,
	.fp2=write,
	.fp3=close
};
void main()
{
	int fd;
	char buf[32];
	//fd=open("abc.txt",O_CREAT|O_RDWR,0777);
	fd=s.fp("abc.txt",O_RDWR);
	if(fd<0)
	{
		perror("open");
		return;
	}
	s.fp2(fd,"Hello good morning\n",19);
	printf("Data written to the file\n");
	lseek(fd,0,SEEK_SET);
	int n=s.fp1(fd,buf,11);
	buf[n]='\0';
	printf("Data read from the file %s\n",buf);
	s.fp3(fd);
}
