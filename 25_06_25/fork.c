#include<stdio.h>
#include<unistd.h>
int y=20;
int main()
{
	pid_t ret;
	int x=10;
	ret=fork();
	if(ret<0)
	{
		perror("fork\n");
		return -1;
	}
	else if(ret>0)
	{
		printf("Parent process x=%d\n",x);
		printf("Parent process y=%d\n",y);
		printf("Address of x: %p\n",&x);
		printf("Address of y: %p\n",&y);
	}
	else
	{
		printf("Child process x=%d\n",x);
		printf("Child process y=%d\n",y);
		printf("Address of x: %p\n",&x);
		printf("Address of y: %p\n",&y);
	}
	return 0;
}
