#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
void my_handler(int signum)
{
	printf("running in my_handler\n");

}
int main()
{
	signal(SIGINT ,my_handler);
	while(1)
	{
		printf("running in main\n");
		sleep(1);
	}

}
