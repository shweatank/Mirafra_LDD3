#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
int main()
{
	pid_t q=fork();
	if(q<0)
	{
		perror("fork");
		return -1;
	}
	else if(q==0)
	{
		execlp("/bin/ls","ls",NULL);
	}
	else
	{
		wait(NULL);
		printf("Child completed\n");
	}
	return 0;
}
