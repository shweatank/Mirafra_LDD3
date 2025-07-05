// Program to find GCD of a number

#include<stdio.h>
int main()
{
	int a,b;
	printf("Enter any two numbers to find GCD:");
	scanf("%d %d",&a,&b);

	while(b)
	{
		int temp=b;
		b=a%b;
		a=temp;
	}
	printf("%d is the GCD of given two numbers\n",a);
	return 0;
}
