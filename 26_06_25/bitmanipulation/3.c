#include<stdio.h>
void main()
{
	int num,i;
	printf("Enter number ");
	scanf("%d",&num);
	printf("Enter i value ");
	scanf("%d",&i);
	int result=num&(~(1<<i));
	printf("%d\n",result);
}
