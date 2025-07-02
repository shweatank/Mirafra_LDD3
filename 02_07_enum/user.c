#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
enum num {mon,tue,wed};
void main()
{
	int fd,result;
	enum num val=tue;
	fd = open("/dev/mychar",O_RDWR);
	if(fd<0)
	{
		perror("Failed to open the file");
		return;
	}
	write(fd,&val,sizeof(val));
	lseek(fd, 0, SEEK_SET);
	read(fd,&result,4);
	printf("Data from driver : %d\n",result);
	close(fd);
}
