#include<stdio.h>
void main()
{
	int num,position,rotation;
	printf("Enter a number ");
	scanf("%d",&num);
	printf("Enter (1 for right or 2 for left) you want to rotate ");
	scanf("%d",&position);
	printf("Enter number of rotations ");
	scanf("%d",&rotation);
	for(int i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
	if(position==1)
	{
		num=(num>>rotation)|(num<<(32-rotation));
	}
	else if(position==2)
	{
		num=(num<<rotation)|(num>>(32-rotation));
	}
	else
	{
		printf("Invalid input\n");
	}
	for(int i=31;i>=0;i--)
	{
		printf("%d",num>>i&1);
	}
	printf("\n");
}
	
