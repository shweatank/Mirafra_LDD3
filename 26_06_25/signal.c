#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
void handler(int signo)
{
	printf("Caught signal\n");
	exit(0);
}
void main()
{
	signal(SIGINT,handler);
	while(1)
	{
		printf("Waiting for signal\n");
	}
}
