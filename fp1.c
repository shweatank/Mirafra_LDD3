//24 June 2025
//calculator using function pointer array
#include<stdio.h>
typedef int(*fptr)(int,int);
int addition(int a, int b)
{
	return a+b;
}
int subtraction(int a, int b)
{
	return a-b;
}
int multiplication(int a,int b)
{
	return a*b;
}
int division(int a,int b)
{
	return a/b;
}
void main()
{
	int a,b,op;
	fptr calculator[4]={addition,subtraction,multiplication,division};
	while(1)
	{
		printf("Enter 0 for addition\n\t1 for subtraction\n\t2 for multiplication\n\t3 for division\n");
		scanf("%d",&op);
		printf("Enter the operands: ");
		scanf("%d%d",&a,&b);
		printf("Result = %d\n",calculator[op](a,b));
	}
}
