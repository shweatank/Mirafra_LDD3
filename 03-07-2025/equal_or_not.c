//Program to check whether two numbers are equal or not without using caomparison operator


#include<stdio.h>
int main()
{
	int num1,num2;
	printf("Enter any two integers to check whether they are equal or not:");
	scanf("%d %d",&num1,&num2);

	if((num1^num2)==0)
	{
		printf("num1 and num2 are equal\n");
	}
	else
	{	
		printf("num1 and num2 are not equal\n");
	}
	return 0;
}
