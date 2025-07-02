#include<stdio.h>
void main()
{
	int num=0,sum=0,temp=0;
	printf("Enter number ");
	scanf("%d",&num);
	temp=num;
	for(int i=1;i<num;i++)
	{
		if(num%i==0)
		{
			sum+=i;
		}
	}
	if(sum>num)
	{
		printf("%d is abundant number\n",temp);
	}
	else
	{
		printf("%d is not an abundant number\n",temp);
	}
}
