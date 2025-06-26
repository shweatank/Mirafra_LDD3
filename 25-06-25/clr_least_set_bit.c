//25 June 2025
//To clear the least significant set bit
#include<stdio.h>
void main()
{
	int num,i;
	printf("Enter the number : ");
	scanf("%d",&num);
	for(i=0;i<32;i++)
	{
		if((num>>i)&1)
		{
			num=num^(1<<i);
			break;
		}
	}
	printf("Result = %d\n",num);
}
