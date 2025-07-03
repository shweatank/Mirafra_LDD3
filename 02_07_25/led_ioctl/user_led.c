#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/ioctl.h>

#define DEVICE_NAME "/dev/led_dev"

#define MAGIC_NUM 'L'
#define SET_BRIGHTNESS _IOW(MAGIC_NUM,1,int)
#define SET_COLOUR _IOW(MAGIC_NUM,2,enum led_colour)

enum led_colour
{
	red=0,
	green=1,
	blue=2
};
int main()
{
	int fd,ret,val=1,read_val;
	fd=open(DEVICE_NAME,O_RDWR);
	if(fd<0)
	{
		perror("message\n");
		exit(0);
	}
	ret=write(fd,&val,sizeof(int));
	if(ret<0)
	{
		perror("message\n");
		exit(0);
	}

	 ret=read(fd,&read_val,sizeof(int));
        if(ret<0)
        {
                perror("message\n");
                exit(0);
        }
        read_val==0 ? printf("LED IS ON\n"):printf("LED IS OFF\n");

	int brightness=23;
	ioctl(fd,SET_BRIGHTNESS,&brightness);
	enum led_colour c=green;
	ioctl(fd,SET_COLOUR,&c);
}






