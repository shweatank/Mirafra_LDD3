#include<stdio.h>
int main()
{
	int num=0,fact=1;
	printf("Enter a number ");
	scanf("%d",&num);
	while(num>0)
	{
		fact=fact*num;
		num--;
	}
	printf("%d\n",fact);
	return 0;
}
