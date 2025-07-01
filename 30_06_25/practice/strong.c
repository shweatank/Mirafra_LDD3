#include<stdio.h>
int factorial(int n)
{
	int fact=1;
	while(n>0)
	{
		fact=fact*n;
		n--;
	}
	return fact;
}
int main()
{
	int num=0,rem=0,sum=0,temp=0;
	printf("Enter a number ");
	scanf("%d",&num);
	if(num<=0)
	{
		printf("Invalid Input\n");
		return -1;
	}
	temp=num;
	while(num>0)
	{
		rem=num%10;
		sum=sum+factorial(rem);
		num/=10;
	}
	if(sum==temp)
	{
		printf("%d is strong number\n",sum);
	}
	else
	{
		printf("%d is not a strong number\n",temp);
	}
	return 0;
}

