#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
struct data_t{
	int a;
	int b;
	char op;
	int result;
};
int main()
{
	struct data_t data;
	int fd;
	char read_buf[100];
	fd=open("/dev/cal_dev",O_RDWR);
	if(fd<0){
		perror("failed to open device");
	return 1;
}
printf("Enter two integers:");
scanf("%d%d",&data.a,&data.b);
printf("Enter option to perform task: a)add b)sub c)mul d)div\n");
scanf(" %c",&data.op);
write(fd,&data,sizeof(struct data_t));
lseek(fd,0,SEEK_SET);
read(fd,&data,sizeof(struct data_t));
printf("Read from driver: %d\n",data.result);
close(fd);
}
