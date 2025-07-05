#include<stdio.h>
int main()
{
	unsigned char num;
	printf("Enter a number\n");
	scanf("%hhu",&num);
	   num= ((num & 0x0F) << 4) | ((num & 0xF0) >> 4);
	printf("%d",num);
}
