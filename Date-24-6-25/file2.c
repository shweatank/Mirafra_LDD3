#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main()
{
	int fd1=open("temp",O_WRONLY,0644);

	char a[20]="hello mirafra";
	write(fd1,a,strlen(a)+1);
	
	close(fd1);

	int fd2=open("temp",O_RDONLY,0644);

	char b[20];

	read(fd2,b,sizeof(b));

	printf("the messege is = %s\n",b);

	close(fd2);
}
