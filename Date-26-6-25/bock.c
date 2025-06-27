#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
void isr(int sig)
{
	printf("signal %d recived\n",sig);
}
int main()
{
	signal(SIGINT ,isr);
	sigset_t block_set;
	sigemptyset(&block_set);
	sigaddset(&block_set,2);

	printf("Blocxking Sigint for 5 sec\n");
	sigprocmask(SIG_BLOCK,&block_set,0);
	sleep(5);

	printf("UNBlocxking Sigint for now\n");
	sigprocmask(SIG_UNBLOCK,&block_set,0);
	while(1)
	{
		pause();
	}
}
