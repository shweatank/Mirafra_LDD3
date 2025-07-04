#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio_ext.h>
#include<sys/wait.h>
int add(int data1,int data2)
{
	exit(data1+data2);
}
int sub(int data1,int data2)
{
	exit(data1-data2);
}
int mul(int data1,int data2)
{
	exit(data1*data2);
}
int divv(int data1,int data2)
{
	if(data2==0)
	{
		printf("invalid data\n");
		exit(0);
	}
	exit(data1/data2);
}
int main()
{
	char ch;
	int data1,data2,status;
	printf("enter two integers:");
	scanf("%d%d",&data1,&data2);
	pid_t ret=fork();
	if(ret==-1)
	{
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(ret==0)
	{
		printf("iam in child process with pid:%d\n",getpid());
		printf("a:add s:sub m:mul d:div\n");
		printf("enter choice:\n");
		__fpurge(stdin);
		scanf("%c",&ch);
		switch(ch)
		{
			case 'a':add(data1,data2);break;
			case 's':sub(data1,data2);break;
			case 'm':mul(data1,data2);break;
			case 'd':divv(data1,data2);break;
			default:printf("invalid choice\n");exit(0);	 
		}
		exit(0);
	}
	else
	{
		wait(&status);
		if(WIFEXITED(status))
		printf("parent:result:%d\n",WEXITSTATUS(status));
	}
}

