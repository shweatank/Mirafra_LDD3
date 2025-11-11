#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
#define N 3
int main()
{
	pid_t pid,childs[3];
	for(int i=0;i<N;i++)
	{
		pid=fork();
		if(pid==-1)
		{
			perror("fork");
			return EXIT_FAILURE;
		}
		else if(pid==0)
		{
			printf("child %d with pid:%d executing execvp\n",i+1,getpid());
			char *argv[]={"ls","-l",NULL};
			execvp("ls",argv);
			printf("execvp failed\n");
			_exit(1);
		}
		else
		{
			childs[i]=pid;
		}
	}
	for(int i=0;i<N;i++)
	{
		int status;
		pid_t p;
		p=waitpid(childs[i],&status,0);
		if(WIFEXITED(status))
			printf("child %d with pid:%d exited with status:%d\n",i+1,p,WEXITSTATUS(status));
	}
		printf("parent with pid done:%d\n",getpid());
}

