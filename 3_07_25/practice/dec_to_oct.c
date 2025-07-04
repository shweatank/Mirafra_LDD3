#include<stdio.h>
void main()
{
	int num=0,rem=0,oct=0,i=1;
	printf("Enter a number ");
	scanf("%d",&num);
	while(num>0)
	{
		rem=num%8;
		oct+=rem*i;
		i=i*10;
		num/=8;
	}
	printf("%d\n",oct);
}
