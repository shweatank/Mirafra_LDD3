#include<stdio.h>
#include<unistd.h>
#include<signal.h>
void handler(int sig)
{
	printf("Handled signal %d\n",sig);
}
void main()
{
	struct sigaction act;
	act.sa_handler=handler;
	act.sa_flags=0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT,&act,NULL);
	while(1)
	{
		printf("Waiting\n");
		sleep(10);
	}
}
