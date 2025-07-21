#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
void main()
{
	int value;
	int fd=open("/dev/gpio_demo",O_WRONLY);
	printf("Enter value ");
	scanf("%d",&value);
	write(fd,&value,sizeof(int));
	close(fd);
}
