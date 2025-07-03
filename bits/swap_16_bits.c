#include<stdio.h>
void printbinary(int num)
{
        int bit=31;
        while(bit>=0)
        {
                if((num>>bit)&1)
                {
                        printf("1");
                }
                else
                {
                        printf("0");
                }
                bit--;
        }
        printf("\n");
}
int swap_16_bits(int num)
{
	return((num>>16)|(num<<16));
}
int swap_nibbles(int num)
{
	return ((num&0x0F)<<4)|((num&0xF0)>>4);
}
int main()
{
	int num=123,x;
	printbinary(num);
	x=swap_16_bits(num);
	printbinary(x);
	x=swap_nibbles(num);
	printbinary(x);
}
