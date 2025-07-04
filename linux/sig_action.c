#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
void my_handler(int signum)
{
	printf("running in my_handler\n");

}
int main()
{
	struct sigaction sa;
	sa.sa_handler=my_handler;
	sa.sa_flags=0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT,&sa,NULL);
	while(1)
	{
		printf("running in main\n");
		sleep(1);
	}

}
