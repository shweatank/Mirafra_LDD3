#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
void main()
{
	int fd=open("/dev/rpi-uart",O_RDWR);
	if(fd<0)
	{
		perror("open failed");
		return;
	}
	int num1,num2,result;
	char op;
	read(fd,&num1,sizeof(int));
	read(fd,&num2,sizeof(int));
	read(fd,&op,sizeof(char));
	switch(op)
	{
		case '+' : result=num1+num2;
			   break;
		case '-' : result=num1-num2;
			   break;
		case '*' : result=num1*num2;
			   break;
		case '/' : result=num1/num2;
			   break;
		default: printf("Invalid input\n");	
			 result=0;
	}
	write(fd,&result,sizeof(int));
	printf("Data sent to x86 %d\n",result);
}
