#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main()
{
	pid_t pid;
	pid=fork();
	if(pid==-1)
	{
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(pid==0)
	{
		printf("child with pid:%d and ppid:%d\n",getpid(),getppid());
		sleep(3);
		printf("child done\n");
		printf("child with pid:%d and new ppid:%d\n",getpid(),getppid());
		sleep(3);
		exit(0);
	}
	else
	{
		printf("parent with pid:%d done\n",getpid());
	}
}
