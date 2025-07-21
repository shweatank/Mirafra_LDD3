#include<stdio.h>
int main()
{
    int num=0x1234;
	int res=(((num&0xff)<<8) |( (num&0xff00)>>8));
	printf("%x\n",res);

}
