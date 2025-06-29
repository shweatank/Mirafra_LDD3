#include<stdio.h>

int Add(int a,int b)
{
	return a+b;
}

int Sub(int a,int b)
{
	return a-b;
}
int Mul(int a,int b)
{
	return a*b;
}
int main()
{
	int num1,num2,ch;
	printf("enter two numbers:");
	scanf("%d %d",&num1,&num2);

	int (*operation)(int,int);

	printf("1.Add 2.Sub 3.Mul\n");
	printf("Enter choice:");
	scanf("%d",&ch);

	switch(ch)
	{
		case 1:operation=Add;
		       break;
		case 2:operation=Sub;
		       break;
		case 3:operation=Mul;
		       break;
	}
	printf("%d",operation(num1,num2));
}
