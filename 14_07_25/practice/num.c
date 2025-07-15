#include<stdio.h>
void main()
{
	int num1=0,num2=0,i=1;
	printf("Enter a number ");
	scanf("%d",&num1);
	while(num1>0)
	{
		int rem=num1%10;
		num2=num2+rem*i;
		if(rem==0)
		{
			num2=num2+1*i;
		}
		i=i*10;
		num1/=10;
	}
	printf("%d\n",num2);
}
