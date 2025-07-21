#include<stdio.h>

int swap_nibbile(int a)
{
	int i,j,k,l;
	i=(a>>0)&(0x0f);
	j=(a>>4)&(0x0f);
	k=(a>>8)&(0x0f);
	l=(a>>12)&(0x0f);
	a=(i<<0)|(k<<4)|(j<<8)|(l<<12);
	return a;
}
int main()
{
	int a=0x1234;
	a=swap_nibbile(a);
	printf("%x",a);
}
