#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<unistd.h>

#define DEVICE "/dev/ioctl_demo"

#define MAJOR_NUM 101
#define IOCTL_SET_NUM _IOW(MAJOR_NUM,0,int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM,1,int)

struct st{
	int a;
	int b;
	char ch;
	int result;
}val;
int main(){
	int fd;
	
	fd=open(DEVICE,O_RDWR);
	if(fd<0){
		perror("failed to open device");
		return 1;
	}
	printf("enter values and operation\n");
	scanf("%d %d", &val.a, &val.b);
	while ((getchar()) != '\n');
	scanf("%c", &val.ch);
	printf("sending values to kernel...\n");
	ioctl(fd,IOCTL_SET_NUM,&val);
	
	printf("Requesting value  from kernel...\n");
	ioctl(fd,IOCTL_GET_NUM,&val);
	
	printf("Received value from kernel %d\n",val.result);
	close(fd);
	return 0;
}
