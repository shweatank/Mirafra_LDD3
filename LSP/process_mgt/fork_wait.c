#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
int main()
{
	pid_t pid;
	pid=fork();
	if(pid==-1)
	{
		perror("fork:");
		return EXIT_FAILURE;
	}
	else if(pid==0)
	{
		printf("child process with pid:%d and ppid:%d\n",getpid(),getppid());
		sleep(3);
		printf("wait done in child\n");
		exit(0);
	}
	else
	{
		printf("parent process with pid:%d\n",getpid());
		wait(NULL);
		printf("parent resuming...\n");
	}
}
