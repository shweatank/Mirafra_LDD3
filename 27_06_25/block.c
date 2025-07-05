#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
void handler(int sig)
{
	printf("signal %d receive \n",sig);
	exit(0);
}
int main()
{
	signal(SIGINT,handler);
	sigset_t block_set;
	sigemptyset(&block_set);
	sigaddset(&block_set,SIGINT);
	printf("blocing sigint for 5 seconds---\n");
	sigprocmask(SIG_BLOCK,&block_set,NULL);
	sleep(5);
	printf("unblocking SIGINT---try pressing ctl+c--\n");
	sigprocmask(SIG_UNBLOCK,&block_set,NULL);
	while(1)
	{
		pause();
	}
}

