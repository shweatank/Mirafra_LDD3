#include<stdio.h>
void main()
{
	int num=0,count=0,rem=0;
	printf("Enter a number ");
	scanf("%d",&num);
	while(num>0)
	{
		count++;
		num/=10;
	}
	printf("%d\n",count);
}
