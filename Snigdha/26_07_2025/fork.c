#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include<stdlib.h>

int main()
{
	pid_t pid=fork();
	if(pid<0)
	{
		perror("Fork failed\n");
		exit(1);
	}
	else if(pid==0)
	{
		printf("Child process\n");
		printf("Child ID:%d\t Parent ID:%d\n",getpid(),getppid());
	}
	else
	{
		printf("Parent process\n");
		printf("Child ID:%d\t Parent ID:%d\n",pid,getppid());
	}
}
