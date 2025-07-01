#include<stdio.h>
int main()
{
	int base=0,exponent=0,power=1;
	printf("Enter base ");
	scanf("%d",&base);
	printf("Enter exponent ");
	scanf("%d",&exponent);
	while(exponent>0)
	{
		power*=base;
		exponent--;
	}
	while(exponent<0)
	{
		power/=base;
		exponent++;
	}
	printf("%d\n",power);
	return 0;
}
