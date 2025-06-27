#include<stdio.h>
#include<unistd.h>
#include<signal.h>
void main()
{
	int pid;
	printf("Enter the process id...\n");
	scanf("%d",&pid);
	kill(pid,SIGINT);
	printf("Enter the process id...\n");
	scanf("%d",&pid);
	kill(pid,SIGQUIT);
}
