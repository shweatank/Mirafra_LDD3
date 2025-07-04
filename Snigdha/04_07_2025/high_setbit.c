#include<stdio.h>


int main()
{
	int n;
	printf("Enter number:");
	scanf("%d",&n);

	int order=-1;

	for(int i=0;i<(sizeof(int)*8);i++)
	{
		if (n&(1<<i))
			order=i;
	}
	if(order!=-1)
		printf("Highest order set bit:%d\n",order);
	else
		printf("No set bits\n");
}
