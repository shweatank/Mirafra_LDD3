#include<stdio.h>
struct functions {
int (*Add)(int,int);
int (*Sub)(int,int);
int (*Mul)(int,int);
int (*Div)(int,int);
};
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
int main()
{
	int num1,num2,op,result;
struct functions cal={
	.Add=add,
	.Sub=sub,
	.Mul=mul,
	.Div=div,
};
	printf("Enter two numbers:");
	scanf("%d%d",&num1,&num2);
	printf("Result of Add=%d\n",cal.Add(num1,num2));
	printf("Result of Sub=%d\n",cal.Sub(num1,num2));
	printf("Result of Mul=%d\n",cal.Mul(num1,num2));
	printf("Result of Div=%d\n",cal.Div(num1,num2));
}

