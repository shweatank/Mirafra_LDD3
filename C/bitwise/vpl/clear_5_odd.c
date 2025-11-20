#include<stdio.h>
void print_binary(int num)
{
	for(int bit=31;bit>=0;bit--)
	{
		printf("%d",(num>>bit)&1);
		if(bit%8==0)
			printf(" ");
	}
	printf("\n");
}
int clear_bits(int num,int n)
{
	int mask=0;
	for(int i=0;i<n;i++)
	{
		mask|=(1<<((2*i)+1));
	}
	return num & ~mask;
}
int main()
{
	int num;
	printf("enter a num:\n");
	scanf("%d",&num);
	int n;
	printf("enter how many odd positions:\n");
	scanf("%d",&n);
	print_binary(num);
	num=clear_bits(num,n);
	printf("after clear:\n");
	print_binary(num);
}
