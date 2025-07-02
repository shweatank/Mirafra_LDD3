#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
void main()
{
	int fd=open("/dev/simple_char_dev",O_RDWR);
	if(fd<0)
	{
		perror("open error");
		return;
	}
	int buf[4]={0};
	printf("Enter two numbers ");
	scanf("%d %d",&buf[0],&buf[1]);
	printf("Enter operation (1.add, 2.sub, 3.mul, 4.Div) ");
	scanf("%d",&buf[2]);
	write(fd,(char *)buf,3*sizeof(int));
	lseek(fd,0,SEEK_SET);
	read(fd,(int *)buf,4*sizeof(int));
	printf("%d\n",buf[3]);
	close(fd);
}
