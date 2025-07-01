#include<stdio.h>
int fun(int n)
{
	int len=0;
	while(n>0)
	{
		len++;
		n/=10;
	}
	return len;
}
int main()
{
	int num=0,temp=0,sum=0;
	printf("Enter a number ");
	scanf("%d",&num);
	if(num<0)
	{
		printf("Number would be greater than 10\n");
		return -1;
	}
	temp=num;
	int count=fun(num);
	while(num>0)
	{
		int r=1;
		int c=count;
		int rem=num%10;
		for(int i=0;i<count;i++)
		{
			r=r*rem;
		}
		sum=sum+r;
		num/=10;
	}
	if(temp==sum)
	{
		printf("armstrong number\n");
	}
	else
	{
		printf("Not an armstrong number\n");
	}
	return 0;
}
