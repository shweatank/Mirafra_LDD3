//30 June 2025
//swapping nibbles
#include<stdio.h>
void main()
{
	int num,result;
	printf("Enter the result : ");
	scanf("%x",&num);
	result=(num&0xf00f)|((num&0xf00)>>4)|((num&0xf0)<<4);
	printf("%x\n",result);
}
