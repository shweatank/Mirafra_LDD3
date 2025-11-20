#include<stdio.h>
int set_bit_cnt(unsigned int num)
{
	if(num<=0)
		return 0;
	return 1+set_bit_cnt(num&(num-1));
}
int main()
{
	unsigned int num;
	printf("enter num:\n");
	scanf("%d",&num);
	int cnt=set_bit_cnt(num);
	printf("cnt:%d\n",cnt);
}
