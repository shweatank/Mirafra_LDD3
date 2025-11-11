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
		return 	EXIT_FAILURE;
	}
	else if(pid==0)
	{
		printf("child with pid:%d\n",getpid());
		exit(0);
	}
	else
	{
		printf("parent with pid:%d\n",getpid());
		sleep(10); //during this child becomes zombie
	}
}

