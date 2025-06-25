#include<stdio.h>
typedef int (*fp)(int,int);
struct op
{
	fp add;
	fp sub;
	fp mul;
	fp div;
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

int main()
{
	struct op cal;
	cal.add=add;
	int a=add(10,20);
	printf("%d\n",a);
}
