// To Find LCM of two numbers

#include<stdio.h>

int check_GCD(int a,int b)
{
	while(b)
	{
		int temp=b;
		b=a%b;
		a=temp;
	}
	return a;
}
int check_LCM(int a,int b)
{
	return (a*b)/check_GCD(a,b);
}
int main()
{
	int num1,num2;
	printf("Enter two numbers to calculate LCM:");
	scanf("%d %d",&num1, &num2);

	printf("%d is the LCM of %d and %d\n",check_LCM(num1,num2),num1,num2);
	return 0;
}
