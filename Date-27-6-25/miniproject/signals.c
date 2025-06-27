#include<stdio.h>
#include<signal.h>
int main()
{
	int pid;
	printf("Enter The PID To Send To Signal To Process1:\n");
	scanf("%d",&pid);
	kill(pid,2);
	printf("to process1 SIGINT is delivered\n");
	printf("Enter The PID To Send To Signal To Process1:\n");
	scanf("%d",&pid);
	kill(pid,3);
	printf("to process1 SIGQUIT is delivered\n");

}
