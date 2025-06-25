#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
struct Fileops{
	int (*openfile)(const char *);
	void (*writefile)(int,const char *);
	void (*readfile)(int);
};
int open_file(const char *filename)
{
	int fd=open(filename,O_CREAT|O_RDWR,0644);
	if(fd<0)
	{
		perror("open");
		return 0;
	}
	return fd;
}
void write_file(int fd,const char *text){
	if(write(fd,text,strlen(text))<0)
	{
		perror("write");
		return;
	}
}
void read_file(int fd)
{
	char buffer[100];
	lseek(fd,0,SEEK_SET);
	int r=read(fd,buffer,sizeof(buffer)-1);
	if(r<0)
	{
		perror("read");
                return;
	}
	buffer[r]='\0';
	printf("Data :%s\n",buffer);
}
int main()
{
	char *file="file.txt";
	char *message="How are you?";
	struct Fileops fops={
		.openfile=open_file,
		.writefile=write_file,
		.readfile=read_file
	};
	int fd=fops.openfile(file);
	fops.writefile(fd,message);
	fops.readfile(fd);
	close(fd);
}
