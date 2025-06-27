#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
void handle_sigint(int sig)
{
	printf("%d\n",sig);
}
void main()
{
	struct sigaction sa;
	sa.sa_handler=handle_sigint;
	sa.sa_flags=0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT,&sa,NULL);
	while(1)
	{
		printf("hello\n");
		sleep(1);
	}
}
