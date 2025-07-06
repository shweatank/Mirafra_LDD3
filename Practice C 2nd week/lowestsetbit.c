#include<stdio.h>
void main()
{
	int num,i;
	printf("enter the number : ");
	scanf("%d",&num);
	for(i=0;i<32;i++)
		if((num>>i)&1)
			break;
	if(i==32)
		printf("No set bit\n");
	else
		printf("%d has lowest set bit at %d position\n",num,i);
}
