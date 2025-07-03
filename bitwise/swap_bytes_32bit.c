#include<stdio.h>
unsigned  int to_swap(unsigned int num)
{
	unsigned  int byte1,byte2,byte3,byte4;
	byte1=(num>>0)&0x000000ff;
	byte2=(num>>8)&0x000000ff;
	byte3=(num>>16)&0x000000ff;
	byte4=(num>>24)&0x000000ff;
	return (byte1<<24)|(byte2<<16)|(byte3<<8)|(byte4<<0);
}
int main()
{
	unsigned int num;
	printf("enter  int:");
	scanf("%x",&num);
	printf("before swap:0x%08x\n",num);
	printf("after swap:0x%08x\n",to_swap(num));
}
