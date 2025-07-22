#include<stdio.h>
void main()
{
	int num;
	printf("Enter a number ");
	scanf("%d",&num);
	if(num&1<<31)
	{
		printf("%d is negative number\n",num);
	}
	else
	{
		printf("%d is positive number\n",num);
	}
}
