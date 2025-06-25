#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

//int x=5;
int main()
{
	int x=5;
	pid_t pid=fork();
	if(pid==0)
	{
		printf("Child Process\n");
		printf("%p\n%d\n",&x,x);
	}
	else if(pid<0)
	{
		printf("Process not created\n");
		exit(1);
	}
	else
	{
		printf("Parent process:%d\nAddress:%p",x,&x);
	}
}

