#include<stdio.h>
void main()
{
	int num1=0,num2=0,i=0,max=0,hcf=1;
	printf("Enter a number ");
	scanf("%d",&num1);
	printf("Enter a number ");
	scanf("%d",&num2);
	max=num1>num2? num1:num2;
	for(i=2;i<max;i++)
	{
		if(num1%i==0 &&num2%i==0)
		{
			hcf=i;
		}
	}
	printf("HCF of %d and %d is %d\n",num1,num2,hcf);
}
