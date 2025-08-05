#include<stdio.h>

int main()
{
	int a,b,carry;
	scanf("%d %d",&a,&b);
	while(b!=0)
	{
		carry=a&b;
		a=a^b;
		b=carry<<1;
	}
	printf("%d\n",a);
	return 0;
}

