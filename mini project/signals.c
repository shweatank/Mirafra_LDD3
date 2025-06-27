#include<stdio.h>
#include<unistd.h>
#include<signal.h>
void main()
{
	int pid;
	printf("Enter the process id...\n");
	scanf("%d",&pid);
	kill(pid,SIGINT);/*sends first signal to the process2 parent for starting reading data*/
	printf("Enter the process id...\n");
	scanf("%d",&pid);
	kill(pid,SIGQUIT);/*sends first signal to the process2 parent for starting reading result*/
}
