#include<stdio.h>
unsigned short int to_swap(unsigned int num)
{
	unsigned short int byte1,byte2;
	byte1=(num>>0)&0x00ff;
	byte2=(num>>8)&0x00ff;
	return (byte1<<8)|(byte2);
}
int main()
{
	unsigned short int num;
	printf("enter short int:");
	scanf("%hx",&num);
	printf("before swap:0x%04x\n",num);
	printf("after swap:0x%04x\n",to_swap(num));
}
