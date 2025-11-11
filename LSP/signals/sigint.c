#include<stdio.h>
#include<signal.h>
#include<unistd.h>
void my_handler(int signum)
{
	printf("sigint caught with signum:%d and ignoring\n",signum);
}
int main()
{
	signal(SIGINT,my_handler);
	printf("try pressing cntl+c\n");
	for(int i=0;i<5;i++)
	{
		printf("sleeping %d\n",i+1);
		sleep(1);
	}

	printf("prog done\n");
}
