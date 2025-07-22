#include<stdio.h>
void main()
{
	int num;
	printf("Enter a number ");
	scanf("%d",&num);
	if((num&7)==0)
		printf("%d is divisible by 8\n",num);
	else
		printf("%d is not divisible by 8\n",num);
}
