#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
int main()
{
	pid_t pid;
	int fd[2];
	if(pipe(fd)==-1)
	{
		perror("pipe");
		return EXIT_FAILURE;
	}
	pid=fork();
	if(pid==-1)
	{
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(pid)
	{
		close(fd[0]);
		char arr[]="swarna";
		int len=strlen(arr)+1;
		if(write(fd[1],arr,len)==len)
			printf("data written to pipe successfully\n");
		else
			printf("failed to write data into pipe\n");
		close(fd[1]);
	}
	else
	{
		close(fd[1]);
		char ch,i=0,arr[10];
		if(read(fd[0],arr,sizeof(arr))>0)
			printf("data read from pipe:%s\n",arr);
		else
			printf("failed to read data from pipe\n");
		close(fd[0]);
	}
}

