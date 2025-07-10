#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main()
{
	int *p=(int *)malloc(20*sizeof(int));

	if(fork()==0)
	{
		printf("in child process\n");
		printf("the pid=%d ppid=%d\n",getpid(),getppid());
		while(1);
	}
	else
	{
		printf("in parent process\n");
		printf("the pid=%d ppid=%d\n",getpid(),getppid());
		while(1);
	}
}

