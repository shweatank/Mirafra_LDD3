#include<stdio.h>
void main()
{
	int num,i;
	printf("Enter a number ");
	scanf("%d",&num);
	printf("Enter position of bit you want to toggle ");
	scanf("%d",&i);
	num=num^(1<<i);
	printf("%d\n",num);
}
