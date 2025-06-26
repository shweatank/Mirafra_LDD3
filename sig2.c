#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
void isr__(int sig)
{
	printf("hello from isr\n");

}
int main()
{
	struct sigaction sa;
	sa.sa_handler=isr__;
	sa.sa_flags=0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT,&sa,NULL);
	while(1)
	{
		printf("hello from  main\n");
		sleep(1);
	}
}

