#include<stdio.h>
typedef struct
{
	int (*Add)(int,int);
	int (*Sub)(int,int);
	int (*Mul)(int,int);
	int (*Div)(int,int);
}Cal;
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
	printf("Enter two numbers:");
	scanf("%d%d",&num1,&num2);
	Cal c;
	printf("Enter op:1)add 2)sub 3)mul 4)div\n");
	scanf("%d",&op);
	switch(op)
	{
		case 1:c.Add=add;
		       result=c.Add(num1,num2);
		       printf("Result=%d\n",result);
		       break;
		case 2:c.Sub=sub;
		       result=c.Sub(num1,num2);
		       printf("Result=%d\n",result);
		       break;
		case 3:c.Mul=mul;
		       result=c.Mul(num1,num2);
		       printf("Result=%d\n",result);
		       break;
		case 4:c.Div=div;
		       result=c.Div(num1,num2);
		       printf("Result=%d\n",result);
		       break;
	}
}

