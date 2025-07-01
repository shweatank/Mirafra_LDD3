#include<stdio.h>
int main()
{
	int num1=0,num2=0,sum=0,temp=0,rem=0;
	printf("Enter first number ");
	scanf("%d",&num1);
	printf("Enter second number ");
	scanf("%d",&num2);
	if(num1<100 || num1>999 || num2<100 ||num2>999||num1>num2)
	{
		printf("Invalid input\n");
		return -1;
	}
	for(int i=num1;i<=num2;i++)
	{
		int j=temp=i;
		sum=0;
		while(j>0)
		{
			rem=j%10;
			sum=sum+rem*rem*rem;
			j/=10;
		}
		if(temp==sum)
		{
			printf("%d is armstrong number\n",temp);
		}
	}
	return 0;
}
