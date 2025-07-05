#include<stdio.h>
#include<unistd.h>
int add(int x,int y)
{
	return(x+y);
}
int sub(int x,int y)
{
	return(x-y);
}
int main()
{
	int ret,x=0,a=23,b=13,status;
	char ch;
	ret=fork();
	if(ret<0)
	{
		perror("message");
		exit(0);
	}
	else if(ret==0)
	{
		printf("i am the child process\n");
		printf("entre the choice:\n");
		scanf("%c",&ch);
		if(ch=='1')
		{
			x=add(a,b);
			printf("%d",x);
			exit(x);

		}
		else if(ch=='2')
		{
			x=sub(30,21);
			exit(x);
		}
	}
	else
	{
		printf("i am the parent\n");
		wait(&status);
		printf("the result of the operation is:%d\n",(status>>8)&0xff);
	}
}
