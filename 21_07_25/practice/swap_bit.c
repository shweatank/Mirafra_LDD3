#include<stdio.h>
void main()
{
	int num,i,j;
	printf("Enter a number ");
	scanf("%d",&num);
	printf("Before swapping ");
	for(i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
	printf("Enter ith position ");
	scanf("%d",&i);
	printf("Enter jth position ");
	scanf("%d",&j);
	int bit1=num>>i&1;
	int bit2=num>>j&1;
	if(bit1!=bit2)
	{
		int mask=(1<<i)|(1<<j);
		num=num^mask;
	}
	printf("After swapping ");
	for(i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
}
