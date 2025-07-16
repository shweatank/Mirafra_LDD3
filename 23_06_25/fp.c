#include<stdio.h>
int add(int a,int b)
{
	return a+b;
}
int sub(int a,int b)
{
	return a-b;
}
int mul(int a,int b)
{
	return a*b;
}
int div(int a,int b)
{
	return a/b;
}
int func(int x,int y,int index,int (*fptr[])(int,int))
{
	return fptr[index](x,y);
}
void main()
{
	int a=20,b=10,i=2;
	int (*fptr[])(int,int)={add,sub,mul,div};
	printf("%d\n",func(a,b,i,fptr));
}
