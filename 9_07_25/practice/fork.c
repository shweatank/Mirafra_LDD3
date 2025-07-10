#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<stdlib.h>
void main()
{
	pid_t q=fork();
	int *a=(int *)malloc(sizeof(int));
	*a=20;
	if(q==0)
	{
		printf("child pid %d\n",getpid());
		*a=30;
		printf("Value in child process %d\n",*a);
	}
	else
	{
		wait(NULL);
		printf("Parent pid %d\n",getpid());
		printf("Value in parent process %d\n",*a);
	}
	sleep(10);
}
