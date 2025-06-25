#include<stdio.h>
#include<unistd.h>
int main()
{
	int x=0;
	printf("%d %d\n",x,getpid());

	while(1)
	{
	printf("%d\n",x);
	}
}

