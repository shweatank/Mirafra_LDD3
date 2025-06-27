#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
void isr(int num)
{
	printf("The signal caught ctrl+c %d\n",num);
}
int main()
{
	struct sigaction sa;
	sa.sa_handler=isr;
	sa.sa_flags=0;
	sigemptyset(&sa.sa_mask);
	sigaction(2,&sa,NULL);

	while(1)
	{
		printf("running process..\n");
		sleep(2);
	}
}
