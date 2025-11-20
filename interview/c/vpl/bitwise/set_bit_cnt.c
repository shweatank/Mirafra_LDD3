#include<stdio.h>
int set_bit_cnt(int num)
{
	int cnt=0;
	while(num)
	{
		cnt++;
		num&=(num-1);
	}
	return cnt;
}
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
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	int cnt=set_bit_cnt(num);
	print_binary(num);
	printf("cnt:%d\n",cnt);
}
