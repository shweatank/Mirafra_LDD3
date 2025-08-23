#include<stdio.h>
void to_print(unsigned int num)
{
	for(int bit=31;bit>=0;bit--)
		printf("%d",num>>bit&1);
	printf("\n");
}
int to_toggle(unsigned int num)
{
	for(int bit=30;bit>=0;bit--)
	{
		num^=(1<<bit);
	}
	return num;
}
int main()
{
	unsigned int num,res;
	scanf("%u",&num);
	to_print(num);
	res=to_toggle(num);
	to_print(res);
}
