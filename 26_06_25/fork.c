#include<stdio.h>
#include<unistd.h>
int x=32;
int main()
{
	int y=50,ret;
	ret=fork();
	if(ret==-1)
	{
		perror("message");
	}
	else if(ret==0)
	{
		printf("i am the child\n");
		printf("%d  %d\n",x,y);
		printf("%p address %p\n",x,y);

		printf("%d %d\n",getpid(),getppid());
	}
	else
	{
		printf("i am the parent\n");
		wait(NULL);
		 printf("%d %d\n",x,y);
                printf("%p address %p\n",x,y);
		printf("%d %d\n",getpid(),getppid());
	}
}

