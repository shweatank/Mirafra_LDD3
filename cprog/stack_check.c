#include<stdio.h>

int main()
{
	int a,b;
	if((&b-&a)>0)
	{
		printf("stack grows up\n");
	}
	else
	{
		printf("stack grows down\n");
	}
	return 0;
}
