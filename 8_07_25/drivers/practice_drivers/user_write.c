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
	printf("Enter two numbers ");
	scanf("%d %d",&s.x,&s.y);
	printf("Enter your operator ");
	scanf(" %c",&s.op);
	int fd=open("/dev/calc",O_RDWR);
	write(fd,&s,sizeof(s));
	close(fd);
}
