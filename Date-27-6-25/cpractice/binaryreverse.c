#include<stdio.h>
void reverse_binary(int *x)
{
	for(int i=31,j=0;i>j;i--,j++)
	{
		if((*x>>i&1)!=(*x>>j&1))
		{
			*x=*x^(1<<i);
			*x=*x^(1<<j);
		}
	}
}
int main()
{
	int num;
	printf("Enter The Number:\n");
	scanf("%d",&num);
	for(int i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
	reverse_binary(&num);
	for(int i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
}
