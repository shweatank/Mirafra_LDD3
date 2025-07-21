#include<stdio.h>
extern int (*fp)(int,int);
int main()
{
	int a=4,b=5,c;
	c=fp(a,b);
	printf("c=%d\n",c);
}
