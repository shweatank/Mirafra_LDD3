#include<stdio.h>

int main()
{
	int a,b,c=0;
	scanf("%d %d",&a,&b);
	c|=(a|b)|((a&b)<<1);
	printf("%d\n",c);
}

