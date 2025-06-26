#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
int main()
{
	pid_t pid;
	pid=fork();//crreates child process
	if(pid<0)//fork() syscall failed
	{
		fprintf(stderr,"E:Fork failed\n");
		return 1;
	}
	else if(pid==0)//child process
	{
		execlp("/bin/ls","ls",NULL);
	}
	else//parent process
	{
		wait(NULL);//parent will wait for the child to complete
		printf("I:Child completed.\n");
	}
	return 0;
}
