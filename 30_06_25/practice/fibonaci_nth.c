#include<stdio.h>
int main()
{
	int num=0,a=0,b=1,c=0;
	printf("Enter a number ");
	scanf("%d",&num);
	if(num<=0)
	{
		printf("Invalid Input\n");
		return -1;
	}
	if(num==1)
	{
		printf("%d\n",a);
	}
	else if(num==2)
	{
		printf("%d",b);
	}
	else
	{
		for(int i=2;i<num;i++)
		{
			c=a+b;
			a=b;
			b=c;
		}
		printf("%d\n",c);
	}
	return 0;
}


