#include<stdio.h>
void to_print(unsigned int num)
{
	for(int bit=31;bit>=0;bit--)
	{
		printf("%d ",num>>bit&1);
	}
	printf("\n");
}
void to_check(unsigned int num,int n)
{
	unsigned int bit1,bit2;
	if((num>>n)&1)
	{
		num&=~((1<<12)|(1<<13));
		bit1=(num>>27)&1;
		bit2=(num>>28)&1;
		num|=((bit1<<12)|(bit2<<13));
	}
	to_print(num);
}
int main()
{
	unsigned int num;
        scanf("%u",&num);
 to_print(num);
 to_check(num,5);
}
