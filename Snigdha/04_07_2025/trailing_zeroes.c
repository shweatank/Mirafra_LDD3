#include<stdio.h>


int main()
{
	int n;
	printf("Enter number:");
	scanf("%d",&n);

	int c=0;

	for(int i=0;i<(sizeof(int)*8);i++)
	{
		if (n&(1<<i))
			break;
		c++;
	}
		printf("Number of trailing zeroes:%d\n",c);
}
