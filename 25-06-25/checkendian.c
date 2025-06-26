//25 June 2025
//program to check little endian or big endian
#include<stdio.h>
typedef union union_def
{
	int num;
	char ch;
}checkunion;
void main()
{
	checkunion u;
	u.num=10;
	if(u.num==u.ch)
		printf("Little Endian...\n");
	else
		printf("Big Endian...\n");
}
