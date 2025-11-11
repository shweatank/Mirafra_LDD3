#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<wait.h>
void child_handler(int signum)
{
	printf("sigint caught in child...ignoring...\n");
	fflush(stdout);
}
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
		printf("child process with pid:%d\n",getpid());
		signal(SIGINT,child_handler);
		printf("SIGINT registered press cntl+c\n");
		for(int i=0;i<10;i++)
		{
			printf("child running...\n");
			fflush(stdout);
			sleep(1);
		}
		printf("child done\n");
		exit(0);
	}
	else
	{
		printf("parent process with pid:%d\n",getpid());
		wait(NULL);
		printf("parent finished\n");
	}
}
