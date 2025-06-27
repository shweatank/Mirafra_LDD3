//block signal
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
void handler(int sig){
	printf("signal %d received\n",sig);
}
int main()
{
	signal(SIGINT,handler);
	sigset_t block_set;
	sigemptyset(&block_set);
	sigaddset(&block_set,SIGINT);
	printf("Blocking SIGINT for 5 sec..\n");
	sigprocmask(SIG_BLOCK,&block_set,NULL);
	sleep(5);
	printf("Unblock\n");
	sigprocmask(SIG_UNBLOCK,&block_set,NULL);
	while(1){
		pause();
	}
}
