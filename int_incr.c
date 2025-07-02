#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

#define DEVICE "/dev/inc_dev"

int main(){
	int fd=open(DEVICE, O_RDWR);
        ssize_t n;
	if(fd<0){
		perror("open");
		return 1;
	}

	char input[20],output[20];

	printf("enter the integer num: ");
	scanf("%s",input);

	write(fd, input, strlen(input));
	n=read(fd, output, sizeof(output)-1);
	output[n]= '\0';

	printf("increment num :%s\n",output);

	close(fd);
	return 0;
}


