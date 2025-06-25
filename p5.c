#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
int main()
{
	char text[50];
	
	char buffer[100];
	int fd=open("data.txt",O_CREAT|O_RDWR,0644);
	if(fd==0)
	{
		perror("open");
		return 0;
	}
	printf("Enter data:");
	scanf("%s",text);
	write(fd,text,strlen(text));
	lseek(fd,0,SEEK_SET);

	int r=read(fd,buffer,sizeof(buffer)-1);
	buffer[r]='\0';
	printf("Data:%s\n",buffer);
	close(fd);
}


