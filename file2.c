//24 June 2025
//file operations with function pointers in structure
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
typedef struct file
{
	/*FILE *(* open)(const char *,const char *);
	ssize_t (*read)(void *, size_t, size_t, FILE *);
	//ssize_t (*read)(void *, size_t,  size_t,  FILE *);
	ssize_t (*write)(const void *, size_t, size_t, FILE *);
	int (*close)(FILE *);*/
	int (*open)(const char *,int,...);
	ssize_t (*read)(int,void *,size_t);
	ssize_t (*write)(int,const void *,size_t);
	int (*close)(int);
}fileop;
void main()
{
	char ch;
	fileop files={.open=open,.read=read,.write=write,.close=close};
	int fd=files.open("text1.txt",O_RDWR);
	files.write(fd,"Hyderabad",9);
	lseek(fd,0,SEEK_SET);
	files.read(fd,&ch,1);
	printf("%c\n",ch);
	files.close(fd);
}
