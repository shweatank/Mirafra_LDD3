#include<stdio.h>
void main()
{
	int num=0,square=0,count=0,temp=0;
	printf("Enter a number ");
	scanf("%d",&num);
	temp=num;
	if(num<=0)
	{
		printf("Invalid input\n");
		return;
	}
	while(num>0)
	{
		count++;
		num/=10;
	}
	square=temp*temp;
	int result=0,j=1;
	for(int i=0;i<count;i++)
	{
		result=result+square%10*j;
		square/=10;
		j*=10;
	}
	if(result==temp)
	{
		printf("%d is automorphic number\n",temp);
	}
	else
	{
		printf("%d is not an automorphic number\n",temp);
	}
}
