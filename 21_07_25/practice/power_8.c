#include<stdio.h>
void main()
{
	int num;
	printf("Enter a number ");
	scanf("%d",&num);
	if(num>0 && (num&(num-1))==0 && (num&0x49249249)!= 0)
		printf("%d is power of 8\n",num);
	else
		printf("%d is not power of 8\n",num);
}
