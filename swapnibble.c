#include<stdio.h>
void swapnibble(short int *i)
{
	short int n1,n2,n3,n4;
	n1=(*i>>12)&0xf;
        n2=(*i>>8)&0xf;
	n3=(*i>>4)&0xf;
	n4=(*i)&0xf;
	*i=(n1<<12)|(n3<<8)|(n2<<4)|(n1);
}
int main()
{
	short int i=0x1234;
	printf("i/p:%x",i);
	swapnibble(&i);
	printf("o/p:%x",i);
}
