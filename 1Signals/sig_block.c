#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int sig)
{
	printf("Signal %d Recevied\n",sig);
}

int main()
{
	signal(SIGINT,handler);

	sigset_t block_set;
	sigemptyset(&block_set);

	sigaddset(&block_set,SIGINT);

	printf("BLOCKING SIGINT for 5 seconds\n");
	sigprocmask(SIG_BLOCK,&block_set,NULL);
	sleep(5);

	printf("UNBLOCKING !!!\n");
	sigprocmask(SIG_UNBLOCK,&block_set,NULL);

	while(1)
	{
		pause();
	}
}
