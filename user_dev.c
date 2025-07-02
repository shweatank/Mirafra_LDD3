#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

#define DEVICE "/dev/cal_driver"

int main(){
	int fd,a,b;
	char input[10],output[10],op[10];
	ssize_t n;

	fd=open(DEVICE,O_RDWR);
	if(fd<0){
		perror("open");
		return 1;
	}

	printf("enter the a,b,op values: ");
	scanf("%d %d %s",&a,&b,op);

	snprintf(input, sizeof(input),"%d %d %s",a,b,op);
	write(fd, input, strlen(input));

	n=read(fd, output, sizeof(output)-1);
	output[n]='\0';

	printf("result :%s\n",output);

	close(fd);
	return 0;

}

