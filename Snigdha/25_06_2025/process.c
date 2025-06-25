#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int a=8;
int main()
{
	pid_t pid=fork();
	if(pid<0)
	{
		fprintf(stderr,"fork failed\n");
		return 1;
	}
	else if(pid==0)
	{
		execlp("/bin/ls","ls",NULL);
		printf("%d\n",a);
	}
	else
	{
		wait(NULL);
		printf("Child completed\n");
	}
}
