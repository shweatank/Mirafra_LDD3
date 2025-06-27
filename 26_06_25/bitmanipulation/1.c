#include<stdio.h>
void main()
{
	int num,i;
	printf("Enter number ");
	scanf("%d",&num);
	printf("Enter i value ");
	scanf("%d",&i);
	int result=num&(1<<i);
	if(result!=0)
	{
		printf("set\n");
	}
	else
	{
		printf("unset\n");
	}
}
