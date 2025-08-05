#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 100

int main()
{
	int pipefd[2];
	char write_msg[]="Hello from parent";
	char read_msg[BUF_SIZE];

	pid_t pid=fork();
	if(pid<0)
	{
		perror("Fork failed\n");
		return 1;
	}
	else if(pid==0)
	{
		close(pipefd[1]); // Close write end. Only read
		read(pipefd[0],read_msg,sizeof(read_msg));
		printf("Child received:%s\n",read_msg);
		close(pipefd[0]);
	}
	else
	{
		close(pipefd[0]); // Close read end. Only write
		write(pipefd[1],write_msg,strlen(write_msg)+1);
		printf("Parent sent:%s\n",write_msg);
		close(pipefd[1]);
	}
}
