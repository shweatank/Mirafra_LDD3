//Write a C program to count total zeros and ones in a binary number.

#include<stdio.h>
int main()
{
	int n,count=0;
	scanf("%d",&n);
	while(n)
	{
		count++;
		n=n&(n-1);
	}
	printf("set bits=%d\tclearbits =%d\n",count,32-count);
}
