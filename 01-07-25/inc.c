#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
int main()
{
	int fd;
	char read_buf[100];
	char *write_str;

	fd=open("/dev/increment",O_RDWR);
	if(fd<0){
		perror("failed to open device");
	return 1;
}
printf("Enter num:");
scanf("%s",write_str);
write(fd,write_str,strlen(write_str));
lseek(fd,0,SEEK_SET);
read(fd,read_buf,sizeof(read_buf));
printf("Read from driver: %s\n",read_buf);
close(fd);
}
