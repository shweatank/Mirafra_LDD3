#include<stdio.h>


/*
int main()
{
	unsigned int n;
	scanf("%u",&n);
	int c=0;
	for(int i=31;i>=0;i--)
	{
		if(n &(1<<i))
			c++;
	}
	printf("%d\n",c);
	return 0;
}*/


int main()
{
	unsigned int n;
	scanf("%u",&n);
	int c=0;

	while(n)
	{
		n = n&(n-1);
		c++;
	}
	printf("%d\n",c);
}
