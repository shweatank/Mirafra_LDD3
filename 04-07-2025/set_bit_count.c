#include<stdio.h>
int set_bit_cnt(int num)
{
	int cnt=0;
	for(int bit=31;bit>=0;bit--)
	{
		if((num>>bit)&1)
			cnt++;
	}
	return cnt;
}
void print_binary(int num)
{
	for(int bit=31;bit>=0;bit--)
		printf("%d",(num>>bit)&1);
	printf("\n");
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	print_binary(num);
	printf("set bit count:%d\n",set_bit_cnt(num));
}
