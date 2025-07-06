#include<stdio.h>

int main()
{
	int a,count=0;
	printf("enter number: \n");
	scanf("%d",&a);
	while(a)
	{
		count++;
		a=(a&(a-1));
	}
	printf("set bit count:%d\n",count);
}
