#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
struct operator
{
	int x;
	int y;
	int result;
	char op;
}s;
void main()
{	
	int fd=open("/dev/calc",O_RDWR);
	read(fd,&s,sizeof(s));
	printf("Done\n");
	close(fd);
}
