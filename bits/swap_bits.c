#include<stdio.h>
void print_binary(int num)
{
	int bit=31;
	while(bit>=0)
	{
		if((num>>bit)&1)
		{
			printf("1");
		}
		else
		{
			printf("0");
		}
		bit--;
	}
}
int swap(int a,int b,int n)
{
	int bit1=(n>>a)&1;
	int bit2=(n>>b)&1;
	if(bit1!=bit2)
	{
		n=n^(1<<a);
		n=n^(1<<b);
	}
	printf("\n");
	return n;
}
int main()
{
	int i,j,n,x;
	printf("enter the num\n");
	scanf("%d",&n);
	printf("enter the i and j values:\n");
	scanf("%d %d",&i,&j);
	print_binary(n);
	x=swap(i,j,n);
	print_binary(x);
}
