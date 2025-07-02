#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<unistd.h>
struct data 
{
	int num1;
	int num2;
	char op[4];
};
#define DEVICE "/dev/ioctl_calculator"
#define MAJOR_NUM 100
#define IOCTL_SET_NUM _IOW(MAJOR_NUM,0,int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM,1,int)

int main()
{
	int fd,result=0;
	//int val=1234;
	//int read_val = 0;
	struct data calcu;
	printf("Enter the operands and operator : ");
	scanf("%d%d %s",&calcu.num1,&calcu.num2,calcu.op);
	fd = open(DEVICE,O_RDWR);
	if(fd<0)
	{
		perror("Failed to open device");
		return 1;
	}
	//printf("Sending value %d to kernel..\n",val);
	ioctl(fd,IOCTL_SET_NUM,&calcu);

	printf("Requesting value from kernel...\n");
	ioctl(fd,IOCTL_GET_NUM,&result);

	printf("Received value from kernel :%d\n",result);
	close(fd);
	return 0;
}
