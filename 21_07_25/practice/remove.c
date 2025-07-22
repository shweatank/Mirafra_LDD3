//Remove last set bit
#include<stdio.h>
void main()
{
	int num;
	printf("Enter a number ");
	scanf("%d",&num);
	num=num&num-1;
	printf("%d\n",num);
}
