#include<stdio.h>
void main()
{
	int num,i,j;
	printf("Enter a number ");
	scanf("%d",&num);
	printf("Before reversing ");
	for(i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
	for(i=31,j=0;i>=j;i--,j++)
	{
		int bit1=num>>i&1;
		int bit2=num>>j&1;
		if(bit1!=bit2)
		{
			num=num^((1<<i)|(1<<j));
		}
	}
	printf("After reversing ");
	for(i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
}
