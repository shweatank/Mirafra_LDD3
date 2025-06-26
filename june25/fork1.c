#include<stdio.h>
#include<unistd.h>
int x=10;
void main()
{
	int y=10;
	int ret;
	ret=fork();
	if(ret==0)
	{
		printf("Global in child = %d , %ld\n",x,&x);
		printf("Local in child = %d , %ld\n",y,&y);
	}
	else
	{
		printf("Global in parent = %d , %ld\n",x,&x);
		printf("local in parent = %d , %ld\n",y,&y);
	}
}
