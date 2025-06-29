#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int x=5;
int main()
{
	int ret;
	ret=fork();
	if(ret)
	{
		printf("In Parent\n");
		printf("x=%d Address of x:%p\n",x,&x);
	}
	else if(ret==0)
	{
		printf("In Child\n");
		printf("x=%d Address of x:%p\n",x,&x);
	}
	else
	{
		printf("Failed to create\n");
	}
	return 0;
}
