#include<stdio.h>

int main()
{
	int n;
	printf("enter number:");
	scanf("%d",&n);
	if(!(n&(n-1)))
		printf("%d is power of 2\n",n);
	else
		printf("%d is not power of 2\n",n);
}
