#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
void child_handler(int signum)
{
	printf("child recieved SIGUSR1 from the parent\n");
}
int main()
{
	pid_t pid;
	pid=fork();
	if(pid==-1)
	{
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(pid==0)
	{
		signal(SIGUSR1,child_handler);
		printf("child with pid:%d waiting for signal from parent\n",getpid());
		while(1)
			pause();
	}
	else 
	{
		sleep(2); //giving time to child to register signal
		printf("parent with pid:%d\n",getpid());
		kill(pid,SIGUSR1);
		sleep(1);
		printf("parent exiting and killing the child\n");
		kill(pid,SIGKILL);
	}
}

