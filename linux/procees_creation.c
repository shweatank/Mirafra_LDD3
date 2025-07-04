#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
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
		execlp("bin/ls","ls",NULL);

	}
	else
	{
		wait(NULL);
	printf("child completed\n");

	}
	return 0;
}
