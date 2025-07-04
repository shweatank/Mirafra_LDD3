#include<stdio.h>
int main()
{
	int a,b,c;
	printf("Enter The a and b Values:\n");
	scanf("%d %d",&a,&b);

	b=~b+1;

	while(b!=0)
	{
		c=(a&b)<<1;
		a=a^b;
		b=c;
	}

	printf("The Result is a-b=%d\n",a);
}

