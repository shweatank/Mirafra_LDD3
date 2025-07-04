#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
void my_handler(int signum)
{
	printf("running in my_handler\n");
	sleep(5);
	printf("SIGINT done...\n");
}
void sigquit_handler(int signum)
{
	printf("running siguit_handler\n");
	sleep(1);
	printf("sigquit done...\n");
	exit(0);
}
int main()
{
	struct sigaction sa;
	sa.sa_handler=my_handler;
	sa.sa_flags=0;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGQUIT);
	sigaction(SIGINT,&sa,NULL);
	sa.sa_handler=	sigquit_handler;
	sigaction(SIGQUIT,&sa,NULL);
	while(1)
	{
		printf("running in main\n");
		sleep(1);
	}

}
