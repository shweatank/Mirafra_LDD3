#include<stdio.h>
#include<signal.h>
#include<unistd.h>
void my_handler(int signum)
{
	printf("timer expired...sigalrm caught with signum:%d\n",signum);
}
int main()
{
	signal(SIGALRM,my_handler);
	printf("setting 5 sec\n");
	alarm(5);
	/*for(int i=0;i<10;i++)
	{
		printf("working ...\n");
		sleep(1);
	}*/
	while(1);
	printf("prog done\n");
}
