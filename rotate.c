//Write a C program to input a number and rotate bits of number using bitwise shift operators. How to rotate bits of a given number using bitwise operator in C programming. Logic to left or right rotate bits of a number using bitwise shift operator in C program.

#include<stdio.h>

int leftrotate(int n)
{
	int LSB=n&1;
	for(int i=0;i<32;i++)
	{
		if(n<31)
		{
		if(((n>>i)&1)!=((n>>(i+1))&1))
		{
			n^=(1<<i);
		}
		}
		if(n==31)
		{
			if(((n>>i)&1)!=LSB)
			{
				n^=(1<<i);
			}
		}
	}
	return n;
}

int main()
{
	int n,r;
	printf("enter number :");
	scanf("%d",&n);
	printf("enter no of rotations :");
	scanf("%d",&r);
	for(int i=0;i<r;i++)
	{
		n=leftrotate(n);
		printf("after %d left rotation n=%d",i,n);
	}
}
