#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{
	int fd;
	void (*fp[])()={wrte,read};
	fd=open("ex.txt",O_WRONLY,0664);
	char wstr[100];
	char rstr[100];
	printf("Enter the data:\n");
	fgets(wstr,100,stdin);
	//write(wstr,sizeof(char),strlen(wstr)+1,fp);
	write(fd,wstr,strlen(wstr)+1);
	close(fd);
	fd=open("ex.txt",O_RDONLY);
	read(fd,rstr,sizeof(rstr));
	printf("%s\n",rstr);
	close(fd);
}


