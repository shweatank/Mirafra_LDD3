#include<stdio.h>
int main()
{
	unsigned short num=1234;
	num=(num<<8) |( num>>8);
	printf("%x\n",num);

}

