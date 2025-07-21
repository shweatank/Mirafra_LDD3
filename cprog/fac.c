// Write a C program for calculating the factorial of a number
#include<stdio.h>

int myfact(int n)
{
	if(n==1)
		return 1;
	return n*myfact(n-1);
}

int main()
{
	int n;
	scanf("%d",&n);
	printf("factorial of %d is %d\n",n,myfact(n)); 
}
