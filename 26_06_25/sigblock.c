#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
void handler(int sig)
{
	printf("Signal %d received\n",sig);
}
void main()
{
	signal(SIGINT,handler);
	sigset_t block_set;
	sigemptyset(&block_set);
	sigaddset(&block_set,SIGINT);
	printf("Blocking SIGINT for 5 seconds\n");
	sigprocmask(SIG_BLOCK,&block_set,NULL);
	sleep(5);
	printf("Unblocking SIGINT Try pressing ctrl+c\n");
	sigprocmask(SIG_UNBLOCK,&block_set,NULL);
	while(1)
	{
		pause();
	}
}
