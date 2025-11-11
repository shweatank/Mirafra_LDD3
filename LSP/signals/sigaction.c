#include<stdio.h>
#include<unistd.h>
#include<signal.h>
void quit_handler(int num)
{
	for(int i=0;i<10;i++)
	{
		printf("sigquit caught\n");
		fflush(stdout);
		sleep(1);
	}
	printf("sigquit done\n");
}
void alarm_handler(int num)
{
	printf("sigalrm caught\n");
}
int main()
{
	struct sigaction s1,s2;
	s1.sa_handler=alarm_handler;
	s1.sa_flags=0;
	sigemptyset(&s1.sa_mask);
	sigaction(SIGALRM,&s1,NULL);
	
	s2.sa_handler=quit_handler;
	s2.sa_flags=0;
	sigemptyset(&s2.sa_mask);
	sigaddset(&s2.sa_mask,SIGALRM);
	sigaction(SIGQUIT,&s2,NULL);

	printf("process id:%d\n",getpid());
	printf("press cntl+/ or alarm will fire in 3 sec\n");
	alarm(3);
	while(1)
		pause();
}
