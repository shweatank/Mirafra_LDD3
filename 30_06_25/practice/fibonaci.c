#include<stdio.h>
int main()
{
	int num=0,a=0,b=1,c;
	printf("Enter a number ");
	scanf("%d",&num);
	if(num<=0)
	{
		printf("Invalid input\n");
		return -1;
	}
	if(num==1)
	{
		printf("0\t");
		return 0;
	}
	if(num==2)
	{
		printf("0\t1\t");
		return 0;
	}
	printf("%d\t%d\t",a,b);
	for(int i=2;i<num;i++)
	{
		c=a+b;
		printf("%d\t",c);
		a=b;
		b=c;
	}
	printf("\n");
	return 0;
}
