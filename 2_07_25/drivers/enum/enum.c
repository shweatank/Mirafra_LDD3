#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
void main()
{
	enum day{sunday,monday,tuesday,wednesday,thursday,friday,saturday};
	enum day d;
	enum day e=tuesday;
	int fd=open("/dev/mymodule",O_RDWR);
	if(fd<0)
	{
		perror("open error");
		return;
	}
	write(fd,&e,sizeof(e));
	lseek(fd,0,SEEK_SET);
	read(fd,&d,sizeof(d));
	printf("%d\n",e);
	close(fd);
}
