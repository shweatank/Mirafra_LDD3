#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
struct operation
{
	int a;
	int b;
	char op;
	int result;
}s;
void main()
{
	printf("Enter two number ");
	scanf("%d %d",&s.a,&s.b);
	printf("Enter operation ");
	scanf(" %c",&s.op);
	int fd=open("/dev/mymod",O_RDWR);
	if(fd<0)
	{
		perror("open error");
		return;
	}
	write(fd,&s,sizeof(s));
	lseek(fd,0,SEEK_SET);
	read(fd,&s,sizeof(s));
	printf("Result is %d\n",s.result);
	close(fd);
}
