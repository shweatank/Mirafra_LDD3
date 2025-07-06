//Write a C program to count trailing zeros in a binary number.

#include<stdio.h>

int main()
{
	int n,count=0;
	scanf("%d",&n);
	while(((n>>count)&1)==0)
	{
		count++;
	}
	printf("tailing zeros:%d\n",count);
}
