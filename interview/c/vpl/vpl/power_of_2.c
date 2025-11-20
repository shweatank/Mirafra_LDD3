#include<stdio.h>
int power_of_2(int n)
{
	if((n&(n-1))==0)
		return 1;
	return 0;
}
int main()
{
	int n;
	printf("enter n:\n");
	scanf("%d",&n);
	if(power_of_2(n))
		printf("yes\n");
	else
		printf("no\n");
}
