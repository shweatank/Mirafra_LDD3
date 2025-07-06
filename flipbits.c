//Write a C program to count leading zeros in a binary number.

#include<stdio.h>
int main()
{
	int n;
	scanf("%d",&n);
	for(int i=0;i<32;i++)
	{
		n^=(1<<i);
	}
	printf("after flip:%d\n",n);
}
