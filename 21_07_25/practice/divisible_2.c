#include<stdio.h>
void main()
{
	int num;
	printf("Enter a number ");
	scanf("%d",&num);
	if((num&1)==0)
		printf("%d is divisible by 2\n",num);
	else
		printf("%d is not divisible by 2\n",num);
}
