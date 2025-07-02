#include<stdio.h>
int sum_of_divisors(int n)
{
	int sum=0;
	for(int i=1;i<n;i++)
	{
		if(n%i==0)
		{
			sum+=i;
		}
	}
	return sum;
}

void main()
{
	int num1=0,num2=0,sum1=0,sum2=0;
	printf("Enter a number ");
	scanf("%d",&num1);
	printf("Enter a number ");
	scanf("%d",&num2);
	sum1=sum_of_divisors(num1);
	sum2=sum_of_divisors(num2);
	if(sum1/num1 == sum2/num2)
	{
		printf("%d , %d are friendly pair\n",num1,num2);
	}
	else
	{
		printf("%d , %d are not friendly pair\n",num1,num2);
	}
}
