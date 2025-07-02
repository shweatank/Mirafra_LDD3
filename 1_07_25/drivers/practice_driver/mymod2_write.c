#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
void main()
{
	int num[4];
	printf("Enter two numbers ");
	scanf("%d %d",&num[0],&num[1]);
	printf("Enter your operation (1.add 2.sub 3.mul 4.div) ");
	scanf("%d",&num[2]);
	int fd=open("/dev/mymod2",O_RDWR);
	if(fd<0)
	{
		perror("open");
		return;
	}
	write(fd,num,12);
	printf("Data written\n");
	read(fd,num,16);
	printf("result is %d\n",num[3]);
	close(fd);
}
