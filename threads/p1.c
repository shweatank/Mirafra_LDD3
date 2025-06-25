#include<stdio.h>
#include<unistd.h>
int y=2;
int main()
{
int x=4;
	int result;
	if((result=fork())==0)
	{
         printf("x value in child:%d %p\n",x,&x);
         printf("y value in child:%d %p\n",y,&y);
	}
	else if(result)
	{
		printf("x value in parent:%d %p\n",x,&x);
         printf("y value in parent:%d %p\n",y,&y);
	}

}
