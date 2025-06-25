#include<stdio.h>

int add(int a, int b)
{
	return a+b;
}


int sub(int a, int b)
{
	return a-b;
}


int mul(int a, int b)
{
	return a*b;
}

int dev(int a, int b)
{
	return a/b;
}


void callbackFunc(int a, int b, int (*function)(int, int))
{
	printf("ans = %d\n", function(a, b));
}

int main()
{
	int (*functions[4])(int, int) = {&add, &sub, &mul, &dev};
	
	int a=10, b=5;
	
	int i=0;
	while(i<4)
	{	
		callbackFunc(a,b,functions[i]);
		i++;
	}
	return 0;
}
