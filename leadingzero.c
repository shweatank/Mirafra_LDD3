//Write a C program to count leading zeros in a binary number.


#include<stdio.h>

int main()
{
	int a,n=31,count=0;
	scanf("%d",&a);
	while(((a>>n--)&1)==0)
	{
		count++;
	}
	printf("leading zeros:%d\n",count);
}
