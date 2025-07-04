#include<stdio.h>
int main()
{
	int a,b,carry;
	printf("Enter The a and b:\n");
	scanf("%d %d",&a,&b);

	while(b!=0)
	{
		carry=(a&b)<<1;
		a=a^b;
		b=carry;
	}
	printf("The result is a+b=%d\n",a);
}

