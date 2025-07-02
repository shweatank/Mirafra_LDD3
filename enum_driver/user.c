#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
enum week{SUN,MON,TUE,WED,THUR,FRI,SAT};
char day[7][10]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
void main()
{
	int fd;
	//char read_data[100];
	fd = open("/dev/enum_driver",O_RDWR);
	if(fd<0)
	{
		perror("Failed to open the file");
		return;
	}
	enum week today=MON,balance;
	write(fd,&today,4);
	lseek(fd,0,SEEK_SET);
	read(fd,&balance,4);
	printf("Data from driver : %d\n",balance);
	printf("Tomorrow is %s\n",day[balance]);
	close(fd);
}
