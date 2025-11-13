#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>

void sig_handler(int sig)
{
	printf("Handled the Signal well SIGINT given\n");
	exit(1);
}
int main()
{
	struct sigaction sa;
	sa.sa_handler  = sig_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGINT,&sa,NULL);

	while(1)
	{
		printf("Waiting for SIGINT....\n");
		sleep(1);
	}
}
