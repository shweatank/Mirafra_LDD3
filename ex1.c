#include<stdio.h>

int main()
{
	int x=4;
	printf("x = %d addr=%ld\n",x,&x);
	while(1)
	{
		printf("x=%d addr=%ld \n",x,&x);
	}
}
