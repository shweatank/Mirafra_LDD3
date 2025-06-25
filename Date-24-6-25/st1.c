#include<stdio.h>
struct op
{
	int (*add)(int,int);
	int (*sub)(int,int);
	int (*mul)(int,int);
	int (*div)(int,int);
};

int add(int a,int b)
{
	return a+b;
}

int mul(int a,int b)
{
	return a*b;
}

int sub(int a,int b)
{
	return a-b;
}

int div(int a,int b)
{
	return a/b;
}

struct op cal = {

	.add=add,
	.sub=sub,
	.mul=mul,
	.div=div,
};

int main()
{
	int a=add(10,20);
	printf("add=%d\n",a);
	int b=sub(20,10);
	printf("sub=%d\n",b);

}
