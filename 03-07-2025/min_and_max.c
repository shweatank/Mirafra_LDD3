//Program to find minimum and maximux in two numbers without using any loop or condition

#include<stdio.h>
#include<stdlib.h>                    //for predefined function abs, stdlib.h is needed
int main()
{
	int a,b;
	printf("Enter any two numbers:");
	scanf("%d %d",&a,&b);

	printf("Maximum number is %d\n",((a+b)+abs(a-b))/2);
	printf("Minimum number is %d\n",((a+b)-abs(a-b))/2);

	return 0;
}
