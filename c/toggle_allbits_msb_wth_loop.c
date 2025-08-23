#include<stdio.h>
void to_print(unsigned int num)
{
	for(int bit=31;bit>=0;bit--)
		printf("%d",num>>bit&1);
	printf("\n");
}
int to_toggle(unsigned int num)
{
	return num ^ 0x7fffffff;
}
int main()
{
	unsigned int num,res;
	scanf("%u",&num);
	to_print(num);
	res=to_toggle(num);
	to_print(res);
}
