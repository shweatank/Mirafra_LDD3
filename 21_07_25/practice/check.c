#include<stdio.h>
void main()
{
	int num,i;
	printf("Enter a number ");
	scanf("%d",&num);
	printf("Enter a bit you want to check whether the bit is set or not ");
	scanf("%d",&i);
	if((num&(1<<i))!=0)
	{
		printf("SET\n");
	}
	else
	{
		printf("UNSET\n");
	}
}
