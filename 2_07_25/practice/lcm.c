#include<stdio.h>
void main()
{
	int num1=0,num2=0,lcm=1,i=0;
	printf("Enter a number ");
	scanf("%d",&num1);
	printf("Enter a number ");
	scanf("%d",&num2);
	int max=num1>num2? num1:num2;
	for(i=max;i<=num1*num2;i++)
	{
		if(i%num1==0 && i%num2==0)
		{
			lcm=i;
			break;
		}
	}
	printf("LCM of %d and %d is %d\n",num1,num2,lcm);
}
