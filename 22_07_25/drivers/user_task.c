#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
void main()
{
	int fd=open("/dev/ttyUSB0",O_RDWR);
	if(fd<0)
	{
		perror("open failed");
		return;
	}
	int num1,num2;
	char op;
	int result;
	printf("Enter first number ");
	scanf("%d",&num1);
	printf("Enter second number ");
	scanf("%d",&num2);
	printf("Enter your operation ");
	scanf(" %c",&op);
	write(fd,&num1,sizeof(int));
	write(fd,&num2,sizeof(int));
	write(fd,&op,sizeof(char));
	lseek(fd,0,SEEK_SET);
	read(fd,&result,sizeof(int));
	printf("Result from raspberrypi %d\n",result);
	close(fd);
}
