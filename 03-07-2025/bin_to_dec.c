// Converting binary to decimal

#include<stdio.h>
int main()
{
	int num,digit,a=1,sum=0;
	printf("Enter binary form of any integer:");
	scanf("%d",&num);
	while(num)
	{
		digit=num%10;
		if(digit==1)
		{
			sum+=a;
		}
		num=num/10;
		a=a*2;
	}
	printf("Decimal form of given integer is %d\n",sum);
}
