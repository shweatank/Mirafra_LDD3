//Find the position of the rightmost set bit in a number

#include<stdio.h>

int main()
{
	int n,count=0;
	printf("enter number:");
	scanf("%d",&n);
	while((n&1)==0)
	{
		n=n>>1;
		count++;
	}
	printf("%d bit is rightmost set bit\n",count);
}
