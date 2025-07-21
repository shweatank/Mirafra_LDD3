#include<stdio.h>
int rev_odd(int num)
{
	for(int i=1;i<32;i=i+2)
	{
		num^=(1<<i);
	}
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
	int num=7;
	print_bin(num);
	num=rev_odd(num);
	print_bin(num);
	return 0;
}
