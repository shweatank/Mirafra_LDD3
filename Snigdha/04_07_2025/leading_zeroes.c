#include<stdio.h>


int main()
{
	int n;
	printf("Enter number:");
	scanf("%d",&n);

	int c=0,msb=1<<((sizeof(int)*8)-1);

	for(int i=0;i<(sizeof(int)*8);i++)
	{
		if (n&(msb>>i))
			break;
		c++;
	}
		printf("Number of trailing zeroes:%d\n",c);
}
