#include<stdio.h>
void main()
{
	int num;
	printf("Enter a number ");
	scanf("%d",&num);
	if((num&num-1)==0&&(num&0x55555555)!=0)
		printf("%d is power of 4\n",num);
	else
		printf("%d is not power of 4\n",num);
}
