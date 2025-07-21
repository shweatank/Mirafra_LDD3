#include<stdio.h>

int set_i_j(int num,int i,int j)
{
	int mask=(~(~0<<(j-i+1)))<<i;
	num&=~mask;
	return num;
}
void print_bin(int num)
{
	for(int i=31;i>=0;i--)
	{
		printf("%d ",(num>>i)&1);
	}
	printf("\n");
}
int main()
{
	int num,i,j;
	scanf("%d %d %d",&num,&i,&j);
	print_bin(num);
	num=set_i_j(num,i,j);
	print_bin(num);
}
