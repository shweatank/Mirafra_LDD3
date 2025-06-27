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
	signal(SIGINT,isr);

	while(1)
	{
		printf("running process..\n");
		sleep(2);
	}
}
