#include<stdio.h>
#include<stdlib.h>

struct functionPointers
{
	int (*add)(int, int);
	int (*sub)(int, int);
	int (*mul)(int, int);
	int (*dev)(int, int);
};

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

int main()
{
	struct functionPointers myFuncs = {
		.add  = add, 
		.sub  = sub, 
		.mul  = mul, 
		.dev  = dev,
	};

	printf("addition = %d\n", myFuncs.add(5, 6));
	printf("substraction = %d\n", myFuncs.sub(7, 3));
	printf("multiplication = %d\n", myFuncs.mul(10, 4));
	printf("devision = %d\n", myFuncs.dev(20, 2));

	return 0;
}
