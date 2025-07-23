#include<stdio.h>
int main()
{
	int num=0x12345678;
	printf("result=0x%0X\n",(num<<24)|((num&0xFF00)<<8)|((num&0XFF0000)>>8)|(num>>24));
}

