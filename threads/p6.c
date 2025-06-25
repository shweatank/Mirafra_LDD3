#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
int main()
{
	pid_t pid;
	pid =fork();
	if(pid<0)
	{
		fprintf(stderr,"E:fork failed\n");
		return 1;
	}
	else if(pid==0)
	{
		execlp("/bin/ls","ls",NULL);
	}
	else
	{
		wait(NULL);
		printf("I:child completed\n");
	}
}
