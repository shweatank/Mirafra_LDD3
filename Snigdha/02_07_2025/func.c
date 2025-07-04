#include<stdio.h>

static inline int add()
{
	int a=1,b=2;
	printf("%d %d\n",a,b);
}
int main()
{
	add();
	add();
	add();
}
