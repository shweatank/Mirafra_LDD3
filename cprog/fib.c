// How to generate fibonacci numbers? How to find out if a given number is a fibonacci number or not? Write C programs to do both.

#include<stdio.h>

int fib(int n)
{
	if(n==1 || n==0)
		return 1;
	return (fib(n-1) + fib(n-2));
}
