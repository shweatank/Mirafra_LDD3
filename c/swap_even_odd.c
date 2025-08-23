#include<stdio.h>
void to_print(int num)
{
	for(int bit=31;bit>=0;bit--)
		printf("%d ",num>>bit&1);
	printf("\n");
}
int to_swap(unsigned int num)
{
unsigned num1,num2;
num1=num&0xAAAAAAAA;
num2=num&0x55555555;
return (num1>>1)|(num2<<1);
}
int main()
{
  unsigned int num,res;
  scanf("%u",&num);
  to_print(num);
  res=to_swap(num);
  to_print(res);
}
