#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
struct operations
{
	int (*file_open)(char *);
	void (*file_close)(int);
	void (*file_write)(int ,char *);
	char* (*file_read)(int);
};
int my_open(char *temp)
{
	int fd=open(temp,O_CREAT|O_RDWR,0644);
	if(fd<0)
	{
		perror("open");
		return 0;
	}
	return fd;
}
void my_close(int fd)
{
	close(fd);
}
void my_write(int fd,char *a)
{
	write(fd,a,strlen(a)+1);
}
char *my_read(int fd)
{
	char *b=(char *)malloc(20*sizeof(char));
	read(fd,b,20);
	return b;

}
struct operations op={
	.file_open=my_open,
	.file_close=my_close,
	.file_write=my_write,
	.file_read=my_read,
};
int main()
{
	int fd=my_open("temp1");

	char a[20]="hi mirafra";
	my_write(fd,a);
	my_close(fd);

	fd=my_open("temp1");
	char *c=(char *)malloc(20*sizeof(char));
	c=my_read(fd);
	printf("The Messege is %s\n",c);
	my_close(fd);
	free(c);
}

