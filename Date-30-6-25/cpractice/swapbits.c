#include<stdio.h>
void print_binary(int n)
{
	for(int i=31;i>=0;i--)
		printf("%d",n>>i&1);
	printf("\n");
}
int main()
{
	int num;
	int i,j;
	printf("Enter The NUmber:\n");
	scanf("%d",&num);
	print_binary(num);

	printf("Enter The i and j postions:\n");
	scanf("%d%d",&i,&j);

	if((num>>i&1)!=(num>>j&1))
	{
		num=num^1<<i;
		num=num^1<<j;
	}

	print_binary(num);
}

