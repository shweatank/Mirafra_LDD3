#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig)
{
	printf("Signal received.. Exiting\n");
	exit(0);
}


int main()
{
	signal(SIGINT,signal_handler);

	while(1)
	{
		printf("Running... Press Ctrl+C to exit\n");
		sleep(1);
	}
}
