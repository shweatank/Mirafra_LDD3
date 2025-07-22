//swapping first and last nibble in a number
#include<stdio.h>
void main()
{
	int num;
	printf("Enter a number ");
	scanf("%d",&num);
	for(int i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
	num=((num&0xf0000000)>>28)|((num&0x0000000f)<<28)|(num&0x0fffffff0);
	for(int i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
}
