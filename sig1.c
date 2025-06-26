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
	while(1)
	{
		printf("hello from  main\n");
		sleep(2);
	}
}

