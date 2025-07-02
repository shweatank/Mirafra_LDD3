#include<stdio.h>
void main()
{
	int num=0,sum=0,temp=0;
	printf("Enter a number ");
	scanf("%d",&num);
	temp=num;
	while(num>0)
	{
		int rem=num%10;
		sum+=rem;
		num/=10;
	}
	if(temp%sum==0)
	{
		printf("%d is harshad number\n",temp);
	}
	else
	{
		printf("%d is not a harshad number\n",temp);
	}
}
