#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
 
#define DEVICE "/dev/enum_dev"

enum colour
{
	red=0,
	green=1,
	blue=2
};
int main()
{
	int fd,ret;
	fd=open(DEVICE,O_WRONLY);
	if(fd<0)
	{
		perror("message:\n");
		exit(0);
	}
	enum colour c=green;
        ret=write(fd,&c,sizeof(c));
	if(ret<0)
	{
		perror("failed\n");
		exit(0);
	}
	printf("sent succesfully to kernel\n");
}
