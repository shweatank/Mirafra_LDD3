#include<stdio.h>
void main()
{
	int num=0x12345678;
	num=((num&0xff000000)>>24)|((num&0x000000ff)<<24)|((num&0x00ff0000)>>8)|((num&0x0000ff00)<<8);
	printf("%p\n",num);
}

