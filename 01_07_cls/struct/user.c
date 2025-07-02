#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
struct calcu 
{
	int n1;
	int n2;
	char ch;
};
void main()
{
	int fd,result;
	struct calcu c;
	//char read_data[100];
	//const char *write_data="Hello from user space!";
	printf("Enter the values: ");
	scanf("%d%d %c",&c.n1,&c.n2,&c.ch);
	fd = open("/dev/mychar",O_RDWR);
	if(fd<0)
	{
		perror("Failed to open the file");
		return;
	}
	write(fd,&c,sizeof(c));
	lseek(fd,0,SEEK_SET);
	read(fd,&result,4);
	printf("Data from driver : %d\n",result);
	close(fd);
}
