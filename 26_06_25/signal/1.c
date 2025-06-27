#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
void handle_sigint(int sig)
{
	printf("sigint\n");
}
void main()
{
	signal(SIGINT,handle_sigint);
	while(1)
	{
		printf("hello\n");
		sleep(1);
	}
}
