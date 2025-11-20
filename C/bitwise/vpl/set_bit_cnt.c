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
int set_bit_cnt(int num)
{
	int cnt=0;
	while(num)
	{
		cnt++;
		num=num&(num-1);
	}
	return cnt;
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	print_binary(num);
	printf("cnt:%d\n",set_bit_cnt(num));
}
