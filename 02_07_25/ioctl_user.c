#include<stdio.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>

#define DEVICE "/dev/ioctl_demo"
#define MAJOR_NUM 100

#define IOCTL_SET_NUM _IOW(MAJOR_NUM,0,int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM,1,int)

int main()
{
	int fd;
	int val=1234;
	int read_val=0;
	fd=open(DEVICE,O_RDWR);
	if(fd<0)
	{
		perror("message\n");
		exit(0);
	}
	printf("sending value %d to kernel....\n",val);
	ioctl(fd,IOCTL_SET_NUM,&val);
	printf("requesting  value from kernel....\n");
        ioctl(fd,IOCTL_GET_NUM,&read_val);
	printf("receive value from kernel:%d....\n",read_val);
        close(fd);
}


	
