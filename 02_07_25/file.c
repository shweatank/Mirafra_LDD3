
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
enum data_t{
	 a,
	 b,
	 c,
	 d
};
int main()
{
	enum data_t data;
	int fd;
	char read_buf[100];
	fd=open("/dev/enum",O_RDWR);
	if(fd<0){
		perror("failed to open device");
	return 1;
}
write(fd,&data,sizeof(enum data_t));
lseek(fd,0,SEEK_SET);
read(fd,&data,sizeof(enum data_t));
printf("Read from driver: %d\n",data);
close(fd);
}
