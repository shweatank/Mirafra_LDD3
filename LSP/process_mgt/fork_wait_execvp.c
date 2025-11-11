#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
int main()
{
	pid_t pid;
	pid=fork();
	if(pid==-1)
	{
		perror("fork:");
		return EXIT_FAILURE;
	}
	else if(pid==0)
	{
		char *argv[]={"ls","-l",NULL};
		printf("child with pid:%d executing ls by execvp\n",getpid());
		execvp("ls",argv);
		printf("execvp failed\n");
		_exit(1);
	}
	else
	{
		int status;
		wait(&status);
		if(WIFEXITED(status))
			printf("child exited with status:%d\n",WEXITSTATUS(status));
		printf("parent process:%d\n",getpid());
	}
}
