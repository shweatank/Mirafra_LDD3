#include<stdio.h>
void main()
{
	int num,i;
	printf("Enter the number:");
	scanf("%d",&num);
	for(i=0;i<32;i++)
		if(((num>>i)&1))
			break;
	printf("Number of trailing zeros = %d\n",i);
}
