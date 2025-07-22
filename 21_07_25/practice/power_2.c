#include<stdio.h>
void main()
{
	int num;
	printf("Enter a number ");
	scanf("%d",&num);
	if((num&num-1)==0)
	{
		printf("%d is power of 2\n",num);
	}
	else
	{
		printf("%d is not power of 2\n",num);
	}
}
