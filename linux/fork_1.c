#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int global_x;
int main()
{
	pid_t ret=fork();
	if(ret==-1)
	{
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(ret==0)
	{
		printf("iam a child process with pid:%d and ppid:%d\n",getpid(),getppid());
		printf("child:value of x:%d",global_x);
		exit(0);
	}
	else
	{
	printf("iam a parent  process with pid:%d and ppid:%d\n",getpid(),getppid());
	printf("parent:value of x:%d\n",global_x);
	exit(0);
	}
}
