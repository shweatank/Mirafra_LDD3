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
	signal(SIGINT,isr__);

	sigset_t block_set;
	sigemptyset(&block_set);
	sigaddset(&block_set,SIGINT);

	printf("Blocking sigint for 5 sec.. \n");
	sigprocmask(SIG_BLOCK,&block_set,NULL);
	sleep(5);

	printf("UNBlocking sigint \n");
        sigprocmask(SIG_UNBLOCK,&block_set,NULL);
        sleep(5);


	while(1)
	{
		pause();
	}
}

