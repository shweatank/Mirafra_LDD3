#include<stdio.h>

int add(int ,int);
int sub(int,int);
int mul(int,int);
int (*fp[3])(int,int)={add,sub,mul};
int call_back(int,int,int);

int main()
{
	int a,b,c,d;
	scanf("%d",&a);
	scanf("%d",&b);
	printf("enter the func to be execute 1.sum 2.sub 3.mul\n");
	scanf("%d",&d);
	d=d-1;
	c=call_back(a,b,d);
	printf("result =%d\n",c);
}
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
int call_back(int a,int b,int c)
{
	return (fp[c](a,b));
}
