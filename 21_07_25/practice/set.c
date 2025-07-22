#include<stdio.h>
void main()
{
	int num,i;
	printf("Enter a number ");
	scanf("%d",&num);
	printf("Enter a bit you want to set ");
	scanf("%d",&i);
	num=num|(1<<i);
	printf("%d\n",num);
}
